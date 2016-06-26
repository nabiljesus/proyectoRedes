#include "stdlib.h"


#define MAXSIZE 202

struct circular_buffer
{
    void * buffer[MAXSIZE]; // Buffer
    int  writer;            // Indice sobre el elemento a escribir
    int  reader;            // Indice sobre el elemento a leer

};

void init_buffer(struct circular_buffer * cb,int size);

inline int its_full(struct circular_buffer cb);

inline void *get_writer(struct circular_buffer  cb);

inline void advance_writer(struct circular_buffer * cb);

inline int its_empty(struct circular_buffer cb);

void * read_cb(struct circular_buffer * cb);

