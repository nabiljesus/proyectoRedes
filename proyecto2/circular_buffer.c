#include "stdlib.h"
#include "circular_buffer.h"

/* Inicializando buffer con apuntadores a structuras de tamano esp.*/
void init_buffer(struct circular_buffer * cb,int size){
    int i;

    cb->writer = 0;
    cb->reader = 0;
    for (i = 0; i < MAXSIZE; ++i)
        cb->buffer[i]    = malloc(size);

}

/* Dejando un elemento libre est'a lleno */
inline int its_full(struct circular_buffer cb){
    return (cb.writer + 1) % MAXSIZE == cb.reader;
}

/* Si escritor y lector apuntan al mismo sitio, el buffer esta' vaci'o */
inline int its_empty(struct circular_buffer cb){
    return cb.writer == cb.reader;
}


/* Obtener elemento sobre el cual esta el escritor */
inline void *get_writer(struct circular_buffer  cb){
    return cb.buffer[cb.writer];
}

/* Avanzar el indice del escritor */
inline void advance_writer(struct circular_buffer * cb){
    cb->writer = (cb->writer + 1) % MAXSIZE ;
}

/* Leer del buffer y avanzar el lector */
void * read_cb(struct circular_buffer * cb){
    void * res = cb->buffer[cb->reader];
    cb->reader = (cb->reader + 1)  % MAXSIZE;
    return res;
}

/* Impresion de estado para realizar pruebas */
/*void print_buffer(struct circular_buffer cb){
    int i;
    printf("writer: %d\nreader:%d\n",cb.writer,cb.reader);
    for (i = 0; i < MAXSIZE; ++i)
        printf("%d ",* ((int *) cb.buffer[i]) );
    printf("\n");
}*/

