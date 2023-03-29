#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <assert.h>

#include "bit.h"
#include "rank_support.h"

rank_support build_rank(bitvector *vec){
    rank_support r;
    r.bv  = vec;
    unsigned long chunk_len = (unsigned long)floor(pow((log(vec->n_bits) / log(2)),2));
    unsigned long subchunk_len = (unsigned long)floor((log(vec->n_bits) / log(2)) / 2);
    unsigned long num_chunk = (unsigned long)ceil((vec->n_bits) / (double)chunk_len);
    unsigned long num_subchunk = (unsigned long)ceil(chunk_len / (double)subchunk_len);
    // printf("chunk len: %ld num chunk: %ld subchunk len: %ld num subchunk: %ld\n",
    //     chunk_len, num_chunk, subchunk_len, num_subchunk);
    r.cumulative = (unsigned long *)calloc(num_chunk, sizeof(unsigned long));
    r.relative = (unsigned int**)malloc(sizeof(unsigned int *) * num_chunk);
    r.chunk_len = chunk_len;
    r.subchunk_len = subchunk_len;
    unsigned int chunk_count = 0, subchunk_count = 0;
    for(unsigned long i = 0; i < num_chunk; i++){
        r.relative[i] = (unsigned int *)calloc(num_subchunk, sizeof(unsigned int));
    }
    for(unsigned long i = 0; i < vec->n_bits; i++){
        int bit = get_bit(vec,i);
        unsigned int row = i / chunk_len, col = (i % chunk_len) / subchunk_len;
        if(col < num_subchunk - 1){
            r.relative[row][col + 1] += bit;
            if(subchunk_count == subchunk_len){
                r.relative[row][col + 1] += r.relative[row][col];
                subchunk_count = 0;
            }
            subchunk_count++;
        }
        r.cumulative[row] += bit;
        if(chunk_count == chunk_len && row > 0){
            r.cumulative[row] += r.cumulative[row - 1];
            chunk_count = 0;
            subchunk_count = 0;
        }
        chunk_count++;
    }
    return r;
}

uint64_t rank1(rank_support r, uint64_t i){
    // make sure subchunk len is not greater than 32 bits
    assert(r.subchunk_len <= 32);
    // make sure i is within range
    assert(i <= r.bv->n_bits);

    // in case i = n_bits
    unsigned long num_chunk = (unsigned long)ceil((r.bv->n_bits) / (double)r.chunk_len);
    unsigned long num_subchunk = (unsigned long)ceil(r.chunk_len / (double)r.subchunk_len);

    // calculate subchunk start and where it should end
    unsigned long chunk = i / r.chunk_len, subchunk = (i % r.chunk_len) / r.subchunk_len;
    if(chunk >= num_chunk){
        chunk = num_chunk - 1;
        subchunk = num_subchunk - 1;
        i = r.bv->n_bits;
    }
    uint64_t subchunk_start = r.chunk_len * chunk + r.subchunk_len * subchunk;
    uint64_t subchunk_start_bv = subchunk_start / 8, subchunk_start_bit = subchunk_start % 8;
    uint64_t bv_index = i / 8, bv_bit = i % 8;

    // printf("subchunk_start_bv = %ld bv_index = %ld\n",subchunk_start_bv , bv_index);
    // printf("subchunk_start_bit = %ld bv_bit = %ld\n",subchunk_start_bit , bv_bit);
    // get cumulative and relative sum
    uint64_t rank = (!chunk)? 0 : r.cumulative[chunk - 1];
    rank += r.relative[chunk][subchunk];
    if(subchunk_start_bit == bv_bit){ return rank;} // when i is at the start of a subchunk

    // get rank inside subchunk
    uint32_t x = 0, c = 3;
    if(subchunk_start_bv < bv_index){
        for(uint64_t i = subchunk_start_bv; i < bv_index; i++){
            x += (r.bv->bits[i] << (c * 8)); c--;
        }
        x += (r.bv->bits[bv_index] << (c * 8));
        x = x >> (32 - (3 - c) * 8 - (8 - bv_bit));
    }
    else {
        x += (r.bv->bits[bv_index] << (24 + subchunk_start_bit));
        // printf("x = %x\n",x);
        x >>= (24 + (8 - (bv_bit - subchunk_start_bit)));
        // printf("x = %x\n",x);
    }
    return rank + __builtin_popcount(x);
}

uint64_t rank_overhead(rank_support r) {
    unsigned long num_chunk = (unsigned long)ceil((r.bv->n_bits) / (double)r.chunk_len);
    unsigned long num_subchunk = (unsigned long)ceil(r.chunk_len / (double)r.subchunk_len);
    uint64_t overhead = 8 * (sizeof(r.chunk_len) + sizeof(r.subchunk_len));             // chunk len overhead
    overhead += 8 * sizeof(r.cumulative) + 8 * sizeof(r.cumulative[0]) * num_chunk;   // pointer to and mem of cumulative 
    overhead += 8 * sizeof(r.relative) + 8 * sizeof(r.relative[0]) * num_chunk;         // pointer to pointers to and pointers to relative
    overhead += 8 * sizeof(r.relative[0][0]) * num_chunk * num_subchunk;                // mem of all relatives
    return overhead;
}

void save_rankfile(rank_support r, FILE *ptr) {
    unsigned long num_chunk = (unsigned long)ceil((r.bv->n_bits) / (double)r.chunk_len);
    unsigned long num_subchunk = (unsigned long)ceil(r.chunk_len / (double)r.subchunk_len);
    size_t numbytes = 0;
    // write bit vector
    fwrite(&(r.bv->n_bits), sizeof(r.bv->n_bits),1,ptr);
    numbytes = fwrite(r.bv->bits,1,ceil(r.bv->n_bits/8.0), ptr);
    if(numbytes != ceil(r.bv->n_bits/8.0)){ perror(NULL); exit(1);}
    // write cumulative
    fwrite(&r.chunk_len,sizeof(r.chunk_len),1,ptr);
    numbytes = fwrite(r.cumulative,sizeof(unsigned long),num_chunk,ptr);
    if(numbytes != num_chunk){ perror(NULL); exit(1);}
    // write relative
    fwrite(&r.subchunk_len,sizeof(r.subchunk_len),1,ptr);
    for(uint64_t i = 0; i < num_chunk; i++){
        numbytes = fwrite(r.relative[i],sizeof(unsigned int),num_subchunk,ptr);
        if(numbytes != num_subchunk){ perror(NULL); exit(1);}
    }
}

void save_rank(rank_support r, char *fname){
    FILE *out = fopen(fname,"wb");
    save_rankfile(r,out);
    fclose(out);
}

rank_support load_rankfile(FILE *ptr){
    rank_support r;
    size_t numbytes = 0;
    // read in bitvector
    bitvector *bv = (bitvector *)malloc(sizeof(bitvector));
    fread(&bv->n_bits,sizeof(bv->n_bits),1,ptr);
    bv->bits = (char*)malloc(ceil(bv->n_bits/8.0));
    numbytes = fread(bv->bits,1,ceil(bv->n_bits/8.0),ptr);
    if(numbytes != ceil(bv->n_bits/8.0)){ perror(NULL); exit(1);}
    r.bv = bv;
    // read in cumulative
    fread(&r.chunk_len,sizeof(r.chunk_len),1,ptr);
    unsigned long num_chunk = (unsigned long)ceil((r.bv->n_bits) / (double)r.chunk_len);
    r.cumulative = (unsigned long *)malloc(sizeof(unsigned long) * num_chunk);
    numbytes = fread(r.cumulative,sizeof(unsigned long),num_chunk,ptr);
    if(numbytes != num_chunk){ perror(NULL); exit(1);}
    // read in relative
    fread(&r.subchunk_len,sizeof(r.subchunk_len),1,ptr);
    unsigned long num_subchunk = (unsigned long)ceil(r.chunk_len / (double)r.subchunk_len);
    r.relative = (unsigned int**)malloc(num_chunk * sizeof(unsigned int *));
    for(uint64_t i = 0; i < num_chunk; i++){
        r.relative[i] = (unsigned int *)malloc(num_subchunk * sizeof(unsigned int));
        numbytes = fread(r.relative[i],sizeof(unsigned int),num_subchunk,ptr);
        if(numbytes != num_subchunk){ perror(NULL); exit(1);}
    }
    return r;
}

rank_support load_rank(char *fname) {
    rank_support r;
    FILE *in = fopen(fname,"rb");
    r = load_rankfile(in);
    fclose(in);
    return r;
}

void print_support(rank_support *r){
    unsigned long num_chunk = (unsigned long)ceil((r->bv->n_bits) / (double)r->chunk_len);
    unsigned long num_subchunk = (unsigned long)ceil(r->chunk_len / (double)r->subchunk_len);
    for(unsigned long i = 0; i < num_chunk; i++){
        printf("bitvector: ");
        for(unsigned long j = 0; j < r->chunk_len && (i * r->chunk_len + j) < r->bv->n_bits; j++){
            if(!((i * r->chunk_len + j)%8)){printf("|");}
            printf("%d", get_bit(r->bv, i * r->chunk_len + j));
        }
        printf("\nrel cum. sum: ");
        for(unsigned long j = 0; j < num_subchunk; j++){
            printf("%d ", r->relative[i][j]);
        }
        printf(" cum. sum: %ld\n", r->cumulative[i]);
    }
}

void free_rank_support(rank_support r){
    free(r.cumulative);
    unsigned long num_chunk = (unsigned long)ceil((r.bv->n_bits) / r.chunk_len);
    for(unsigned long i = 0; i < num_chunk; i++){
        free(r.relative[i]);
    }
    free(r.relative);
    destroy_bitvec(r.bv);
}

