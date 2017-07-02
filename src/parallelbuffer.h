/**
 * parallelbuffer.h
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
/**
 * parallelbuffer.h
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
#ifndef _PARALLELBUFFER_H_
#define _PARALLELBUFFER_H_
#include <semaphore.h>

/* use queen to store the size of each element*/
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
    size_t elementsize;
    size_t maxbytes;
    queen elementsize_queen;
    size_t num_count;

    int pfin;
    char *data;

}ParallelBuffer;


void pb_init(ParallelBuffer *pb, size_t per_size, size_t maxelements);

void pb_free(ParallelBuffer *pb);

void *pb_pop(ParallelBuffer *pb, size_t *size);

void pb_take(ParallelBuffer *pb);

void *pb_push(ParallelBuffer *pb);

void pb_append(ParallelBuffer *pb, size_t size);

int pb_processfin(ParallelBuffer *const pb);

void pb_producefin(ParallelBuffer *pb);

#endif
