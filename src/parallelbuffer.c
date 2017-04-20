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

void pb_init(ParallelBuffer *pb, size_t per_size, size_t maxelements)
{
   pb->maxelements = maxelements;
   pb->per_size = per_size;
   pb->maxbytes = maxelements * per_size;
   pb->data = NULL;
   if ((pb->data = (char *)malloc(pb->maxbytes)) == NULL){
       fprintf(stderr, "Parallel Buffer: Init malloc error\n");
   }
   pb->front = 0;
   pb->rear = 0;
   sem_init(&pb->count, 0, 0);
   sem_init(&pb->capacity, 0, maxelements);
   queen_init(&pb->element_size, maxelements);
}

void pb_free(ParallelBuffer *pb)
{

    sem_destroy(&pb->count);
    sem_destroy(&pb->capacity);
    
    if (pb->data != NULL)
        free(pb->data);
    queen_free(&pb->element_size);

}

void *pb_pop(ParallelBuffer *pb, size_t *size)
{
    sem_wait(&pb->count);
    *size = queen_pop(&pb->element_size);

    return pb->data + pb->front;

}

void pb_take(ParallelBuffer *pb)
{
    pb->front += pb->per_size;
    if (pb->front >= pb->maxbytes){
        pb->front = 0;
    }
    sem_post(&pb->capacity);
}

void *pb_push(ParallelBuffer *pb)
{

    sem_wait(&pb->capacity);

    return pb->data + pb->rear;

}

void pb_append(ParallelBuffer *pb, size_t size)
{
    assert(size <= pb->per_size);

    queen_push(&pb->element_size, size);
    pb->rear += pb->per_size;
    if (pb->rear >= pb->maxbytes){
        pb->rear = 0;
    }
    sem_post(&pb->count);

}
