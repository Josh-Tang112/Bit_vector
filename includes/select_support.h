#ifndef SELECT
#define SELECT

#include <stdint.h>
#include "bit.h"
#include "rank_support.h"

typedef struct {
    rank_support r;
}select_support;

select_support build_select(bitvector *vec);
uint64_t select1(select_support s, uint64_t i);
uint64_t select_overhead(select_support s);
void save_select(select_support s, char *fname);
select_support load_select(char *fname);
void free_select_support(select_support s);

#endif