#ifndef RANK
#define RANK

#include <stdint.h>
#include "bit.h"

typedef struct {
    bitvector* bv;
    unsigned long *cumulative;
    unsigned long chunk_len;
    unsigned int **relative;
    unsigned long subchunk_len;
}rank_support;

rank_support build_rank(bitvector *vec);
uint64_t rank1(rank_support r, uint64_t i);
uint64_t rank_overhead(rank_support r);
void save_rank(rank_support r, char *fname);
void save_rankfile(rank_support r, FILE *ptr);
rank_support load_rank(char *fname);
rank_support load_rankfile(FILE *ptr);
void print_support(rank_support *r);
void free_rank_support(rank_support r);

#endif