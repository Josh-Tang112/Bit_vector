#ifndef BIT
#define BIT

typedef struct {
    char* bits;
    unsigned long n_bits;
}bitvector;

bitvector* create_bitvec(unsigned long n_bits);
void destroy_bitvec(bitvector *vec);
void set_bit(bitvector* vec, int index, int bit);
int get_bit(bitvector* vec, int index);

#endif