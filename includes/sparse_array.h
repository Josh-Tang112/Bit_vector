#ifndef SPARSE
#define SPARSE

#include <stdint.h>
#include <time.h>
#include <stdbool.h>
#include "bit.h"
#include "rank_support.h"

typedef struct {
    char **array;
    rank_support r;
    uint64_t num_elem;
}sparse_array;

// t2 - t1
void sub_timespec(struct timespec t1, struct timespec t2, struct timespec *td);
void create(sparse_array **arr, uint64_t size);
void append(sparse_array *arr, char *elem, uint64_t pos);
void finalize(sparse_array *arr);
bool get_at_rank(sparse_array *arr, uint64_t r, char** elem);
bool get_at_index(sparse_array *arr, uint64_t r, char** elem);
uint64_t get_index_of(sparse_array *arr, uint64_t r);
uint64_t num_elem_at(sparse_array *arr, uint64_t r);
uint64_t size(sparse_array *arr);
uint64_t num_elem(sparse_array *arr);
void save(sparse_array *arr, char *fname);
void load(sparse_array *arr, char *fname);
void free_array(sparse_array *arr);

#endif