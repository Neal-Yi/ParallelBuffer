/**
 * test.c
 * Copyright (c) 2017 orglanss <orglanss@gmail.com>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <stdio.h>
#include <unistd.h>
#include "parallelbuffer.h"

void test_2elements()
{
    int a[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
    ParallelBuffer pb;
    int n = sizeof(a) / sizeof( int);
    pb_init(&pb, sizeof(int), 2); 

#pragma omp parallel sections num_threads(2)
    {
        /*write thread*/
#pragma omp section
        {
          int i;
          int *x;
          for( i = 0; i < n; i++){
              x = (int *)pb_push(&pb);
              *x = a[i];
              pb_append(&pb, sizeof(int));
          }
          /*produce finished*/
          pb_producefin(&pb);
        }
        /*read thread*/
#pragma omp section
        {
          int i = 0;
          int x;
          size_t len;
          while(!pb_processfin(&pb)){
              x = *(int *)pb_pop(&pb, &len);
              printf("element%d:%d size:%lu\n", i++,  x, len);
              pb_take(&pb);

          }
        }
    
    }
    pb_free(&pb);

}

int main()
{
    test_2elements();

}
