#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "bit.h"
#include "rank_support.h"
#include "select_support.h"
#include "sparse_array.h"

int main()
{
    int size = 20;
    char words[10][4] = {"bar","baz","foo","lol","doo","voo","boy","boo","ach","ooh"};
    char *elem;
    sparse_array *arr = NULL;
    create(&arr,size);
    append(arr,words[0],1);
    append(arr,words[4],5);
    finalize(arr);

    int i;
    i = get_at_rank(arr, 1,&elem);
    printf("get_at_rank = %d, %s\n",i, elem);

    i = get_at_index(arr, 5,&elem);
    printf("get_at_index = %d, %s\n",i, elem);

    printf("get_index_of = %ld\n",get_index_of(arr, 2));

    free_array(arr);
    
    
    return 0;
}