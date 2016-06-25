#include "stdlib.h"


#define MAXSIZE 202


struct circular_buffer
{
    void * buffer[MAXSIZE];
    int  writer;
    int  reader;

};

void init_buffer(struct circular_buffer * cb,int size);

/* Leaving 1 empty slot */
inline int its_full(struct circular_buffer cb);

/* Check if boths elements point to the same thing */
int its_empty(struct circular_buffer cb);

/* write to the buffer */
void write_cb(struct circular_buffer * cb,void *element);

/* read from the buffer */
void * read_cb(struct circular_buffer * cb);

