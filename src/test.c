#include <stdio.h>
#include <unistd.h>
#include "parallelbuffer.h"

void test_2elements()
{
    int a[] = {1, 2, 3, 4, 5, 6, 7,8,9, 10,11,12,13,14};
    ParallelBuffer pb;
    int n = sizeof(a) / sizeof( int);
    pb_init(&pb, sizeof(int), 2); 

#pragma omp parallel sections num_threads(2)
    {
#pragma omp section
        {
          int i;
          int *x;
          for( i = 0; i < n; i++){
              x = (int *)pb_push(&pb, sizeof(int));
              *x = a[i];
              pb_append(&pb);
          }
        }
#pragma omp section
        {
          int i;
          int x;
          size_t len;
          for( i = 0; i < n; i++){
              x = *(int *)pb_pop(&pb, &len);
              printf("element%d:%d size:%lu\n", i,  x, len);
              pb_take(&pb);

          }
        }
    
    }

}

int main(int argc, const char *argv[])
{
    test_2elements();

}
