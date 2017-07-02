/**
 * parallelbuffer.c
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
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "parallelbuffer.h"


void queen_init(queen *q, size_t maxelements)
{
    q->capacity = maxelements;
    q->front = 0;
    q->rear = 0;
    q->data = NULL;
    if ((q->data = (size_t *)malloc(sizeof(size_t)* maxelements)) == NULL){
       fprintf(stderr, "Parallel Buffer: Init malloc error\n");
    }
}

void queen_free(queen *q)
{
    if (q->data != NULL)
        free(q->data);

}

static size_t queen_pop(queen *q)
{
    size_t ret = q->data[q->front];

    q->front ++;
    if (q->front == q->capacity)
        q->front = 0;

    return ret;
}

static void queen_push(queen *q, size_t element)
{
    q->data[q->rear] = element;

    q->rear ++;
    if (q->rear == q->capacity)
        q->rear = 0;
}

void pb_init(ParallelBuffer *pb, size_t elementsize, size_t maxelements)
{
   pb->elementsize = elementsize;
   pb->maxbytes = maxelements * elementsize;
   pb->data = NULL;
   if ((pb->data = (char *)malloc(pb->maxbytes)) == NULL){
       fprintf(stderr, "Parallel Buffer: Init malloc error\n");
   }
   pb->front = 0;
   pb->rear = 0;
   pb->num_count = 0;
   pb->pfin = 0;
   sem_init(&pb->count, 0, 0);
   sem_init(&pb->capacity, 0, maxelements);
   queen_init(&pb->elementsize_queen, maxelements);
}

void pb_free(ParallelBuffer *pb)
{

    sem_destroy(&pb->count);
    sem_destroy(&pb->capacity);
    
    if (pb->data != NULL)
        free(pb->data);
    queen_free(&pb->elementsize_queen);

}

void *pb_pop(ParallelBuffer *pb, size_t *size)
{
    sem_wait(&pb->count);
    *size = queen_pop(&pb->elementsize_queen);

    return pb->data + pb->front;

}

void pb_take(ParallelBuffer *pb)
{
    pb->front += pb->elementsize;
    if (pb->front >= pb->maxbytes){
        pb->front = 0;
    }
    pb->num_count --;
    sem_post(&pb->capacity);
}

void *pb_push(ParallelBuffer *pb)
{

    void *ret = pb->data + pb->rear;
    sem_wait(&pb->capacity);
    pb->rear += pb->elementsize;
    if (pb->rear >= pb->maxbytes){
        pb->rear = 0;
    }
    return ret;


}

void pb_append(ParallelBuffer *pb, size_t size)
{
    assert(size <= pb->elementsize);
    queen_push(&pb->elementsize_queen, size);
    pb->num_count ++;
    sem_post(&pb->count);

}

void pb_producefin(ParallelBuffer *pb){
  pb->pfin = 1;
}

int pb_processfin(ParallelBuffer *const pb) {
  return (pb->pfin && (pb->num_count == 0)); 
}
