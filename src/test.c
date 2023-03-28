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
    char words[10][4] = {"bar","baz","foo","lol","doo","voo","boy","boo","ach","ooh"}, elem[4];
    sparse_array *arr = NULL;
    struct timespec t1, t2, delta, tot;
    memset(&tot,0,sizeof(struct timespec));
    append(arr,words[c % 10],k);
    finalize(arr);

    get_at_rank(arr, size[i] / 2,elem);
    printf("get_at_rank = %ld\n",get_at_rank(arr, size[i] / 2,elem));

    clock_gettime(CLOCK_REALTIME,&t1);
    get_at_index(arr, size[i] / 2,elem);
    clock_gettime(CLOCK_REALTIME,&t2);
    sub_timespec(t1,t2,&delta);
    printf("For %d long %f sparse array, it averagely take %ld.%ld sec to get_at_index\n\n",
        size[i],(double)c/size[i],delta.tv_sec,delta.tv_nsec);

    clock_gettime(CLOCK_REALTIME,&t1);
    get_index_of(arr, size[i] / 2);
    clock_gettime(CLOCK_REALTIME,&t2);
    sub_timespec(t1,t2,&delta);
    printf("For %d long %f sparse array, it averagely take %ld.%ld sec to get_index_of\n\n",
        size[i],(double)c/size[i],delta.tv_sec,delta.tv_nsec);

    clock_gettime(CLOCK_REALTIME,&t1);
    num_elem_at(arr, size[i] / 2);
    clock_gettime(CLOCK_REALTIME,&t2);
    sub_timespec(t1,t2,&delta);
    printf("For %d long %f sparse array, it averagely take %ld.%ld sec to num_elem_at\n",
        size[i],(double)c/size[i],delta.tv_sec,delta.tv_nsec);
    free_array(arr);
    
    
    return 0;
}