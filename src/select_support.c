#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <math.h>

#include "select_support.h"

select_support build_select(bitvector *vec) {
    select_support s;
    s.r = build_rank(vec);
    return s;
}

uint64_t select_recursive(rank_support r,uint64_t left, uint64_t right, uint64_t i_rank){
    uint64_t c = (left + right) / 2;
    uint64_t c_rank = rank1(r,c);
    // printf("%ld %ld %ld\n",left,c,right);
    if (left > right){
        return UINT64_MAX;
    }
    else if(i_rank == c_rank){
        return c;
    }
    else if(i_rank < c_rank){ // c's rank > i, i in [l, c-1]
        return select_recursive(r,left,c - 1,i_rank);
    }
    else {
        return select_recursive(r,c + 1, right, i_rank);
    }
}

uint64_t select1(select_support s, uint64_t i) {
    rank_support r = s.r;
    uint64_t index = select_recursive(r,0,r.bv->n_bits,i);
    while(index != UINT64_MAX && index + 1 < r.bv->n_bits && rank1(r,index + 1) == i){
        // printf("%ld %ld\n",index, rank1(r,index + 1));
        index++;
    }
    return index;
}

uint64_t select_overhead(select_support s){
    return rank_overhead(s.r);
}
void save_select(select_support s, char *fname){
    save_rank(s.r,fname);
}

select_support load_select(char *fname){
    select_support s;
    s.r = load_rank(fname);
    return s;
}

void free_select_support(select_support s){
    free_rank_support(s.r);
}
