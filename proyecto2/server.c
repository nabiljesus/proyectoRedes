#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <assert.h>
#include <math.h>

#include <time.h>
#include <string.h>

#include <pthread.h>

#include "circular_buffer.h"


#define RESPONSE_PORT 20683 // Puerto para socket de respuesta
#define BUFFER_LEN    1024  // Longitud del buffer de escritura para el socket

// Constantes de estacionamiento
#define PARKING_LOT_SIZE   200
#define HOUR_PRICE          80
#define HOUR_FRACTION_PRICE 30

// Indicar si la accion es de entrada o salida
#define FULL     2
#define WENT_IN  1
#define WENT_OUT 0

typedef int bool;
#define true  1
#define false 0

// Estructura
struct msg
{
    char in_out;  // Identificador de accion (e/s)
    int  car_id;  // Identificador del ticket
    struct sockaddr_in client; // Dir del que env'ia
};

struct tm * parking_space[PARKING_LOT_SIZE];   // Arreglo de horas de llegada
struct circular_buffer cb;                     // Buffer ciruclar para mensajes


int listen_port;     // Puerto a escuchar
int last_payment;    // U'ltimo pago realizado

/*
 *   Procedimiento que consigue un puesto vacío, almacena el tiempo en 
 *   el que ocurrio y entrega un entero con el numero de ticket asociado 
 * 
 *   @return      N'umero de ticket asociado 
 */
int new_ticket(){
    int i = 0;
    time_t time_data;
    time(&time_data); /* Obtener hora */
    
    /* B'usqueda lineal del primer puesto libre */
    while( i < PARKING_LOT_SIZE && parking_space[i] != NULL) ++i;

    /* Almacenar tiempo actual para el ticket */
    parking_space[i] = (struct tm *) malloc(sizeof(struct tm));
    assert(parking_space[i] != NULL);

    localtime_r(&time_data,parking_space[i]);

    return i;
}

/*
 *   Procedimiento que indica cuando debe pagar un cliente dada una posici'on
 *   del arreglo de tiempos.
 *
 *   @param lot   indica la posici'on en la cual esta almacenado el tiempo  
 *   @return      precio que se debe pagar por el ticket  
 */
int ticket_price(int lot){
    double seconds;
    double hours;

    time_t start_time,final_time;
    time(&final_time);
    start_time = mktime(parking_space[lot]);

    seconds = difftime(final_time,start_time);

    /* Redondeo hacia arriba de horas en estacionamiento */
    hours = (int) ceil((seconds / 60) / 60);

    // Actualizar hora de ticket para posterior impresi'on en log
    localtime_r(&final_time,parking_space[lot]);

    return HOUR_PRICE + (hours > 1 ? (hours-1)* HOUR_FRACTION_PRICE  : 0 );
}


/* 
 *   Respues a los clientes luego de pedir o entregar un ticket.
 *
 *   @param addr     Direcci'on al cual se debe enviar el mensaje
 *   @param info     Cadena de caracteres con el mensaje a enviar
 */
void answerClient(struct in_addr addr, char info[]){
    int socks; /* descriptor a usar con el socket */
    struct sockaddr_in out_addr; /* almacenara la direccion IP y numero de puerto del cliente */
    int sentbytes; /* conteo de bytes a escribir */

    out_addr.sin_family = AF_INET; /* usa host byte order */
    out_addr.sin_port = htons(RESPONSE_PORT); /* usa network byte order */
    out_addr.sin_addr = addr;
    bzero(&(out_addr.sin_zero), 8); /* pone en cero el resto */
    /* enviamos el mensaje */

    /* Creamos el socket */
    if ((socks = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        exit(2);
    }

    sentbytes=sendto(socks,
                    info,
                    strlen(info),
                    0,(struct sockaddr *)&out_addr,
                    sizeof(struct sockaddr));

    if ( sentbytes == -1) {
        perror("sendto");
        exit(2);
    }

    /* cierro socket */
    close(socks);
    
}

/* Escribir bitacora para el ticket i, el cual entra o sale*/
void write_action(char *output_file,int coming_inside,int i){
    char action[8];
    char time_buffer[30];
    FILE *log_file = fopen(output_file, "a");
    
    /* Verificamos que de verdad hay un carro*/
    assert(parking_space[i] != NULL);

    if (coming_inside) strcpy(action,"entrado");
    else        strcpy(action,"salido");

    
    /* Creacion de string para el log*/
    strftime(time_buffer, 30, "%d/%m/%Y %H:%M:%S", parking_space[i]);

    if (coming_inside == 2 ){
        fprintf(log_file
               ,"%s | Carro no pudo entrar por estacionamiento lleno \n"
               ,time_buffer);
        return;
    }

    fprintf(log_file
           ,"%s | carro con ticket #%03d ha %s"
           ,time_buffer
           ,i
           ,action);

    if (! coming_inside) fprintf(log_file,". Pago:%d",last_payment);

    fprintf(log_file,"\n");
    /* Actualizacion de archivo */
    fclose(log_file);

    /* Liberando espacio del carro de ser necesario */
    if (! coming_inside){
        free(parking_space[i]);
        parking_space[i] = NULL;
    }
}

/* Procedimiento del thread. Ciclo para escuchar en puerto y agregar datos a buffer */
void * read_messages(){
    int addr_len, numbytes;
    int status;
    char buf[BUFFER_LEN];
    struct msg *last_message;
    struct sockaddr_in server_address;
    struct sockaddr_in client_addr;
    int sockfd;


    /* Inicializacion de servidor */
    server_address.sin_family      = AF_INET;            
    server_address.sin_port        = htons(listen_port); 
    server_address.sin_addr.s_addr = INADDR_ANY;         
    bzero(&(server_address.sin_zero), 8); 

    /* Socket */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) { perror("socket"); exit(1); }

    /* bind */
    status = bind(sockfd
                 ,(struct sockaddr *)&server_address
                 ,sizeof(struct sockaddr));
    if (status == -1)  { perror("bind"); exit(2); }
    
    addr_len = sizeof(struct sockaddr);

    /* Ciclo indefinido de lectura de socket y escritura en buffer */
    while(1) {
        numbytes = recvfrom(sockfd, buf, 
                            BUFFER_LEN, 0, 
                            (struct sockaddr *)&client_addr,
                            (socklen_t *)&addr_len);

        if ( numbytes == -1 ) {
            perror("recvfrom");
            exit(3);
        }


        buf[numbytes] = '\0';

        /* Obtener posicion de escritura para el buffer circular */
        last_message = (struct msg*) get_writer(cb);
        /* Modificar en memoria */
        last_message->in_out = buf[0];
        last_message->car_id = atoi(&buf[1]);
        /* Copiar direccion del cliente que envia */
        memcpy(&last_message->client,&client_addr,sizeof(struct sockaddr_in));
    
        advance_writer(&cb);
    }

}


int main(int argc, char *argv[])
{
    int free_parking_lots = PARKING_LOT_SIZE;
    char *entrance_log,*exit_log; // Strings for log files
    char msg_buffer[30];
    char time_string[30];
    
    int status;      // Auxiliary to check procedure returning values
    int i;
    int last_ticket;
    struct msg *m;    // To read messages
    pthread_t tid;
    

    if (argc != 7) {
        printf ("Uso: sem_svr -l <puerto_sem_svr> -i <bitácora_entrada> -o <bitácora_salida> \n");
        exit(0);
    }

    /* Port get for listening */
    if      (strcmp("-l",argv[1]) == 0 ) listen_port = atoi(argv[2]);
    else if (strcmp("-l",argv[3]) == 0 ) listen_port = atoi(argv[4]);
    else if (strcmp("-l",argv[5]) == 0 ) listen_port = atoi(argv[6]);
    else {
        perror("Error: No se ha especificado del puerto");
        exit(0);
    }

    if      (strcmp("-i",argv[1]) == 0 ) entrance_log = argv[2];
    else if (strcmp("-i",argv[3]) == 0 ) entrance_log = argv[4];
    else if (strcmp("-i",argv[5]) == 0 ) entrance_log = argv[6];
    else {
        perror("Error: No se ha especificado la bitacora de entrada");
        exit(0);
    }

    if      (strcmp("-o",argv[1]) == 0 ) exit_log = argv[2];
    else if (strcmp("-o",argv[3]) == 0 ) exit_log = argv[4];
    else if (strcmp("-o",argv[5]) == 0 ) exit_log = argv[6];
    else {
        perror("Error: No se ha especificado la bitacora de salida");
        exit(0);
    }
    /* Initialize parking lot */
    for (i = 0; i < PARKING_LOT_SIZE; ++i)
        parking_space[i] = NULL;
    /* initialize circular buffer for messages*/
    init_buffer(&cb,(sizeof(struct msg)));


    /* Iniciando hilo de socket de lectura */ 
    pthread_create(&tid, NULL, read_messages, NULL);
    pthread_detach(tid);

    /* Ciclo indefinido de espera de mensajes, procesamiento y respuesta*/
    while (1){
        // Entrega de tickets y aceptar tickets de salida
        while(free_parking_lots > 0 ) {

            /* Esperar a el socket haya recibido alg'un mensaje */
            while (its_empty(cb)) 
                sleep(1); 
            
            /* Consumir mensaje */
            m = read_cb(&cb);

            if (m->in_out=='e')
            {   
                last_ticket = new_ticket();
                --free_parking_lots;

                strftime(time_string
                        ,30
                        ,"%d%m%Y%H%M"
                        ,parking_space[last_ticket]);

                // formato BDDMMYYYYHHMMSSS con SSS como ticket serial
                sprintf(msg_buffer,"1%s%03d",time_string,last_ticket);

                write_action(entrance_log,WENT_IN,last_ticket);
                answerClient(m->client.sin_addr,msg_buffer); 

            }
            else if (m->in_out=='s' && parking_space[m->car_id] != NULL)
            {
                ++free_parking_lots;

                last_payment = ticket_price(m->car_id);

                sprintf(msg_buffer,"X%d",last_payment);
                answerClient(m->client.sin_addr,msg_buffer);

                write_action(exit_log,WENT_OUT,m->car_id);

                
            }
            else if (m->in_out=='s' && parking_space[m->car_id] == NULL)
                printf("Ese ticket no est'a en circulaci'on, intente de nuevo\n");
            else printf("Accion inesperada\n");
            
        }
        

        /* Indicar que no hay puestos libres o aceptar ticket de salida */
        while(free_parking_lots == 0)
        {   
            while (its_empty(cb)) 
                sleep(1); 

            m = read_cb(&cb);

            if (m->in_out=='e') {
                strftime(time_string
                        ,20
                        ,"%d%m%Y%H%M"
                        ,parking_space[last_ticket]);

                sprintf(msg_buffer,"0%sXXX",time_string);

                answerClient(m->client.sin_addr,msg_buffer);  
                write_action(entrance_log,FULL,last_ticket);
            }
            else if (m->in_out=='s' && parking_space[m->car_id] != NULL){
                ++free_parking_lots;

                last_payment = ticket_price(m->car_id);

                sprintf(msg_buffer,"X%d",last_payment);
                answerClient(m->client.sin_addr,msg_buffer); 

                write_action(exit_log,WENT_OUT,m->car_id);
                
            }
            else if (m->in_out=='s' && parking_space[m->car_id] == NULL)
                printf("Ese ticket no est'a en circulaci'on, intente de nuevo\n");
            else printf("Accion inesperada\n");
        }   
    }

}

