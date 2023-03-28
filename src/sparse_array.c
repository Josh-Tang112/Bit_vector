#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <unistd.h>
#include <ctype.h>

#include "bit.h"
#include "rank_support.h"
#include "select_support.h"
#include "sparse_array.h"

enum { NS_PER_SECOND = 1000000000 };
void sub_timespec(struct timespec t1, struct timespec t2, struct timespec *td) // t2 - t1
{
    td->tv_nsec = t2.tv_nsec - t1.tv_nsec;
    td->tv_sec  = t2.tv_sec - t1.tv_sec;
    if (td->tv_sec > 0 && td->tv_nsec < 0)
    {
        td->tv_nsec += NS_PER_SECOND;
        td->tv_sec--;
    }
    else if (td->tv_sec < 0 && td->tv_nsec > 0)
    {
        td->tv_nsec -= NS_PER_SECOND;
        td->tv_sec++;
    }
}

void create(sparse_array **arr, uint64_t size) {
    *arr = (sparse_array*)malloc(sizeof(sparse_array));
    (*arr)->r.bv = create_bitvec(size);
    (*arr)->array = (char**)malloc(size * sizeof(char*));
    (*arr)->num_elem = 0;
}

void append(sparse_array *arr, char *elem, uint64_t pos) {
    if(pos >= arr->r.bv->n_bits){return;}
    set_bit(arr->r.bv,pos,1); // set corresponding bit in bit vector
    arr->array[arr->num_elem] = (char*)malloc(strlen(elem) + 1);  // allocate memory to store string
    strncpy(arr->array[arr->num_elem],elem,strlen(elem)); // cpy string
    arr->num_elem++;
}

void finalize(sparse_array *arr) {
    char **array = (char**)malloc(arr->num_elem * sizeof(char *));
    memcpy(array,arr->array,arr->num_elem * sizeof(char *));
    free(arr->array);
    arr->array = array;
    arr->r = build_rank(arr->r.bv);
}

bool get_at_rank(sparse_array *arr, uint64_t r, char* elem) {
    if(r <= arr->num_elem){
        if(!r)
            elem = NULL;
        else
            elem = arr->array[r - 1];
        return true;
    }
    else{
        return false;
    }
}
bool get_at_index(sparse_array *arr, uint64_t r, char* elem) {
    if(get_bit(arr->r.bv,r)){
        elem = arr->array[rank1(arr->r,r)];
        return true;
    }
    else{
        return false;
    }
}

uint64_t get_index_of(sparse_array *arr, uint64_t r) {
    if(r > arr->num_elem || !r){
        return UINT64_MAX;
    }
    select_support s;
    s.r = arr->r;
    return select1(s,r-1);
}

uint64_t num_elem_at(sparse_array *arr, uint64_t r) {
    return rank1(arr->r,r) + get_bit(arr->r.bv,r);
}

uint64_t size(sparse_array *arr) {
    uint64_t size = 0;
    size += sizeof(char**) + sizeof(char*) * arr->num_elem; // size of str pointers
    for(uint64_t i = 0; i < arr->num_elem; i++){
        size += (strlen(arr->array[i]) + 1);
    }
    size += rank_overhead(arr->r);
    size += ceil(arr->r.bv->n_bits/8.0) + sizeof(unsigned long) * 2;
    return size;
}

uint64_t num_elem(sparse_array *arr){
    return arr->num_elem;
}

void save(sparse_array *arr, char *fname) {
    FILE *out = fopen(fname, "wb");
    save_rankfile(arr->r,out);
    fwrite(&arr->num_elem,sizeof(uint64_t),1,out);
    size_t numBytes = 0;
    for(uint64_t i = 0; i < arr->num_elem; i++){
        uint64_t len = strlen(arr->array[i]);
        fwrite(&len,sizeof(uint64_t),1,out);
        numBytes = fwrite(arr->array[i],1,len,out);
        if(numBytes != len){perror(NULL); exit(1);}
    }
    fclose(out);
}

void load(sparse_array *arr, char *fname){
    arr = (sparse_array*)malloc(sizeof(sparse_array));
    FILE *in = fopen(fname,"rb");
    arr->r = load_rankfile(in);
    fread(&arr->num_elem,sizeof(uint64_t),1,in);
    arr->array = (char**)malloc(arr->num_elem * sizeof(char*));
    size_t numBytes = 0;
    uint64_t len;
    for(uint64_t i = 0; i < arr->num_elem; i++){
        fread(&len,sizeof(uint64_t),1,in);
        arr->array[i] = (char*)malloc(len + 1);
        numBytes = fread(arr->array[i],1,len,in);
        if(numBytes != len){perror(NULL); exit(1);}
        arr->array[len] = '\0';
    }
    fclose(in);
}

void free_array(sparse_array *arr){
    for(uint64_t i = 0; i < arr->num_elem; i++){
        free(arr->array[i]);
    }
    free(arr->array);
    free_rank_support(arr->r);
    free(arr);
    arr = NULL;
}