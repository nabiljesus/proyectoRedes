#include "stdio.h"
#include "stdlib.h"

#define MAXSIZE 10

struct circular_buffer
{
    void *buffer[MAXSIZE];
    int  writer;
    int  reader;

};

void init_buffer(struct circular_buffer * cb ){
    int i;

    cb->writer = 0;
    cb->reader = 0;
    for (i = 0; i < MAXSIZE; ++i){
        cb->buffer[i]    = malloc(sizeof(i));
        *((int *)cb->buffer[i]) = 0;
    }
}

/* Leaving 1 empty slot */
inline int its_full(struct circular_buffer cb){
    return (cb.writer + 1) % MAXSIZE == cb.reader;
}

/* Check if boths elements point to the same thing */
its_empty(struct circular_buffer cb){
    return cb.writer == cb.reader;
}

/* write to the buffer */
void write(struct circular_buffer * cb,void *element){
    cb->buffer[cb->writer] = element;
    cb->writer = (cb->writer + 1) % MAXSIZE ;
}

/* read from the buffer */
void * read(struct circular_buffer * cb){
    void * res = cb->buffer[cb->reader];
    cb->reader = (cb->reader + 1)  % MAXSIZE;
    return res;
}

/* print state */
void print_buffer(struct circular_buffer cb){
    int i;
    printf("writer: %d\nreader:%d\n",cb.writer,cb.reader);
    for (i = 0; i < MAXSIZE; ++i)
        printf("%d ",* ((int *) cb.buffer[i]) );
    printf("\n");
}

int main(int argc, char const *argv[])
{
    int i;
    int *stuffer;
    struct circular_buffer cb;
    init_buffer(&cb);

    for (i = 0; i < MAXSIZE * 2; ++i){
        if (its_full(cb)) printf("Estoy full, no puedo con el primero\n");
        else {  
            stuffer  = malloc(sizeof(int));
            *stuffer = i;
            write(&cb,stuffer);
        }



        if (its_empty(cb)) printf("No hay nada que comer para el primero\n");
        else printf("reader 1 consumio %d\n",* (int *)read(&cb));

        if (its_empty(cb)) printf("No hay nada que comer para el segundo\n");
        else printf("reader 2 consumio %d\n",* (int *)read(&cb));

        print_buffer(cb);
        printf("\n\n\n");
    }

    return 0;
}