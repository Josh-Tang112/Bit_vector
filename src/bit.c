#include "bit.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

bitvector* create_bitvec(unsigned long n_bits) {
    bitvector* vec = (bitvector*)malloc(sizeof(bitvector));
    unsigned long size = ceil(n_bits/8.0);
    vec->n_bits = n_bits;
    vec->bits = (char*)malloc(size);
    return vec;
}

void destroy_bitvec(bitvector *vec) {
    free(vec->bits);
    free(vec);
}

void set_bit(bitvector* vec, int index, int bit) {
    int byte = index >> 3;
    int n = sizeof(index)*8-3;
    int offset = ((unsigned) index << n) >> n;
    if (bit) {
        vec->bits[byte] |= 1 << (7-offset);
    } else {
        vec->bits[byte] &= ~(1 << (7-offset));
    }
}

int get_bit(bitvector* vec, int index) {
    int byte = index >> 3;
    int n = sizeof(index)*8-3;
    int offset = ((unsigned) index << n) >> n;
    if (vec->bits[byte] & (1 << (7- offset))) {
        return 1;
    } else {
        return 0;
    }
}
