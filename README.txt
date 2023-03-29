This program is built with C.
I use the implementation from this github repo to build the methods that create, set, and access 
the bit vector: https://gist.github.com/codescv/6146378

All of the my code does not support commandline arguments. 

### Building Bit Vector
To build the support data structure for rank or select operation, you need to build a bitvector first. 
Include bit.h first, then use create_bitvector to create a bitvector*.
Use set_bit(bitvector* vec, int i, int b) to set ith bit to b.
Use get_bit(bitvector* vec, int i) to get ith bit.
Use destroy_bitvec(bitvector *vec) to free bitvector*.

###rank_support
Use rank_support build_rank(bitvector *) to create support data structure for rank operation. 
Use rank1(rank_support, uint64_t) to do rank operation. 
Use save_rank(rank_support, char*) to save support data structure to disk.
Use load_rank(char*) to get support data structure from disk.
Use free_rank_support(rank_support) to free support data structure.

###select_support
Use select_support build_select(bitvector *) to create support data structure for select operation. 
Use select1(select_support, uint64_t) to do select operation. 
Use save_select(select_support, char*) to save support data structure to disk.
Use load_select(char*) to get support data structure from disk.
Use free_select_support(select_support) to free support data structure.

###sparse array
Use create(sparse_array **, uint64_t) to create an empty sparse array.
Use appen(sparse_array *, char *, uint64_t i) to add element to array. MUST be added IN ORDER.
Use finalize(sparse_array*) after you have added all elements.
Use get_at_rank(sparse_array *, uint64_t, char**) to get element with specific rank. Rank must be GREATER than zero.
Use get_at_index(sparse_array *, uint64_t, char**) to get element at specific index.
Use get_index_of(sparse_array *, uint64_t ) to get index with specific rank.
Use num_elem_at(sparse_array *, uint64_t) to get number of elements at specific index.
Use size(sparse_array *) to know the size of entire data structure.
Use num_elem(sparse_array *) to know number of elements in the array
Use save(sparse_array *, char *) to save sparse array to disk.
Use load(sparse_array *, char *) to load sparse array from disk.
Use free_array(sparse_array *) to free sparse array.