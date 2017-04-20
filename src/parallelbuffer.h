#ifndef _PARALLELBUFFER_H_
#define _PARALLELBUFFER_H_
#include <semaphore.h>

/* use queen to store the of each element*/
typedef struct queen{
    size_t front;
    size_t rear;
    size_t capacity;
    size_t *data;
}queen;


typedef struct ParallelBuffer{
    sem_t count;
    sem_t capacity;
    size_t front, rear;
    size_t per_size;
    size_t maxelements;
    size_t maxbytes;
    queen element_size;

    char *data;

}ParallelBuffer;


void pb_init(ParallelBuffer *pb, size_t per_size, size_t maxelements);

void pb_free(ParallelBuffer *pb);

void *pb_pop(ParallelBuffer *pb, size_t *size);

void pb_take(ParallelBuffer *pb);

void *pb_push(ParallelBuffer *pb, size_t size);

void pb_append(ParallelBuffer *pb);

#endif
