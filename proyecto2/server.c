#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <assert.h>

#include <time.h>
#include <string.h>

#include <pthread.h>

#include "circular_buffer.h"

// #define SERVER_PORT 4321
#define RESPONSE_PORT 20683
#define BUFFER_LEN 1024

#define PARKING_LOT_SIZE 200
#define HOUR_PRICE 80
#define HOUR_FRACTION_PRICE 30

#define WENT_IN  1
#define WENT_OUT 0

typedef int bool;
#define true 1
#define false 0

struct msg
{
    char in_out;
    int  car_id;
    struct sockaddr_in client; // sender
};

// Store hours in array
struct tm * parking_space[PARKING_LOT_SIZE];
struct circular_buffer cb;

int message[2];

int listen_port;              // Port we will be listening


int new_ticket(){
    int i = 0;
    time_t time_data;
    time(&time_data); /* Get time*/
    
    /* Look for first free spot*/
    while( i < PARKING_LOT_SIZE && parking_space[i] != NULL) ++i;

    /* Store time for that spot */
    parking_space[i] = (struct tm *) malloc(sizeof(struct tm));
    assert(parking_space[i] != NULL);

    localtime_r(&time_data,parking_space[i]);

    return i;
}


bool answerClient(struct in_addr addr, char info[], bool notFull){

    int socks; /* descriptor a usar con el socket */
    struct sockaddr_in out_addr; /* almacenara la direccion IP y numero de puerto del cliente */
    int sentbytes; /* conteo de bytes a escribir */
    char BoolDateTime[19];

    out_addr.sin_family = AF_INET; /* usa host byte order */
    out_addr.sin_port = htons(RESPONSE_PORT); /* usa network byte order */
    out_addr.sin_addr = addr;
    bzero(&(out_addr.sin_zero), 8); /* pone en cero el resto */
    snprintf (BoolDateTime, 1, "%d",notFull);
    strcat(BoolDateTime, info); 
    printf("%s\n", BoolDateTime);
    /* enviamos el mensaje */

    /* Creamos el socket */
    if ((socks = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        exit(2);
    }

    sentbytes=sendto(socks,
                    BoolDateTime,
                    strlen(BoolDateTime),
                    0,(struct sockaddr *)&out_addr,
                    sizeof(struct sockaddr));

    if ( sentbytes == -1) {
        perror("sendto");
        exit(2);
    }
    printf("enviados %d bytes hacia %s\n",sentbytes,inet_ntoa(out_addr.sin_addr));
    /* cierro socket */
    close(socks);
    
}


void write_action(char *output_file,int coming_inside,int door,int i){
    char action[8];
    char time_buffer[30];
    FILE *log_file = fopen(output_file, "a");
    printf("File opened\n");
    // Check there is actually a car in there
    assert(parking_space[i] != NULL);

    if (coming_inside) strcpy(action,"entrado");
    else        strcpy(action,"salido");

    
    strftime(time_buffer, 30, "%d/%m/%Y %H:%M:%S", parking_space[i]);

    fprintf(log_file
           ,"%s | carro con ticket #%d ha %s por la puerta %d \n"
           ,time_buffer
           ,i
           ,action
           ,door);

    fclose(log_file);

    /* Release time pointer and mark as free spot */
    if (! coming_inside){
        free(parking_space[i]);
        parking_space[i] = NULL;
    }
}

void * read_messages(){
    int addr_len, numbytes;
    int status;
    char buf[BUFFER_LEN];
    struct msg *last_message;
    struct sockaddr_in server_address;
    struct sockaddr_in client_addr;
    int sockfd;


    /* Server initialization */
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

    while(1) {
        // printf("Pase \n");
        printf("Esperando datos ....\n");
        numbytes = recvfrom(sockfd, buf, 
                            BUFFER_LEN, 0, 
                            (struct sockaddr *)&client_addr,
                            (socklen_t *)&addr_len);

        if ( numbytes == -1 ) {
            perror("recvfrom");
            exit(3);
        }


        printf("paquete proveniente de : %s\n",inet_ntoa(client_addr.sin_addr));
        printf("longitud del paquete en bytes: %d\n",numbytes);
        buf[numbytes] = '\0';
        printf("el paquete contiene: %s\n", buf);

        last_message = (struct msg*) get_writer(cb);
        last_message->in_out = buf[0];
        last_message->car_id = atoi(&buf[1]);
        memcpy(&last_message->client,&client_addr,sizeof(struct sockaddr_in));
    
        advance_writer(&cb);
    }

}


int main(int argc, char *argv[])
{
    int free_parking_lots = PARKING_LOT_SIZE;
    char *entrance_log,*exit_log; // Strings for log files
    char msg_buffer[20];
    char time_string[20];
    
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


    /* Start reader socket */ 
    pthread_create(&tid, NULL, read_messages, NULL);
    pthread_detach(tid);


    while (1){
        // Entregar tickets y aceptar tickets de salida
        while(free_parking_lots > 0 ) {

            /* Wait for a message to appear*/
            while (its_empty(cb)) 
                sleep(1);  // Esto es mucho D:
            
            /* Cosume message*/
            m = read_cb(&cb);

            if (m->in_out=='e')
            {   
                last_ticket = new_ticket();
                --free_parking_lots;

                strftime(time_string
                        ,20
                        ,"%d%m%Y%H%M"
                        ,parking_space[last_ticket]);

                // format BDDMMYYYYHHMMSSS with SSS as ticket serial
                sprintf(msg_buffer,"1%s%03d\0",m->car_id,time_string);
                printf("%s\n",msg_buffer);

                write_action(entrance_log,WENT_IN,1,last_ticket);
                answerClient(m->client.sin_addr,msg_buffer,0); 

            }
            else if (m->in_out=='s')
            {
                ++free_parking_lots;
                // Se debe crear un ticket con los datos recibdos
                write_action(exit_log,WENT_OUT,1,message[1]);

                
            }
            else printf("wtf?\n");

            
        }
        


        // Rebotar gente hasta encontrar algun ticket de salida
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

                sprintf(msg_buffer,"0%sXXX\0",m->car_id,time_string);
                printf("%s\n",msg_buffer);

                write_action(entrance_log,WENT_IN,1,last_ticket);
                answerClient(m->client.sin_addr,msg_buffer,0);  
            }
            else if (message[0]=='s'){
                ++free_parking_lots;
                // Se debe crear un ticket con los datos recibdos
                write_action(exit_log,WENT_OUT,1,message[1]);
                
            }
            else printf("wtf?\n");

        }
        
    }

}
