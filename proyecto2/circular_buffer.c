#include "stdlib.h"
#include "circular_buffer.h"

void init_buffer(struct circular_buffer * cb,int size){
    int i;

    cb->writer = 0;
    cb->reader = 0;
    for (i = 0; i < MAXSIZE; ++i)
        cb->buffer[i]    = malloc(size);

}

/* Leaving 1 empty slot */
inline int its_full(struct circular_buffer cb){
    return (cb.writer + 1) % MAXSIZE == cb.reader;
}

/* Check if boths elements point to the same thing */
int its_empty(struct circular_buffer cb){
    return cb.writer == cb.reader;
}

/* write to the buffer */
void write_cb(struct circular_buffer * cb,void *element){
    free(cb->buffer[cb->writer]);
    cb->buffer[cb->writer] = element;
    cb->writer = (cb->writer + 1) % MAXSIZE ;
}

/* read from the buffer */
void * read_cb(struct circular_buffer * cb){
    void * res = cb->buffer[cb->reader];
    cb->reader = (cb->reader + 1)  % MAXSIZE;
    return res;
}

/* print state */
/*void print_buffer(struct circular_buffer cb){
    int i;
    printf("writer: %d\nreader:%d\n",cb.writer,cb.reader);
    for (i = 0; i < MAXSIZE; ++i)
        printf("%d ",* ((int *) cb.buffer[i]) );
    printf("\n");
}*/

