#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>

#define RESPONSE_PORT 20683
#define BUFFER_LEN 1024

typedef int bool;
#define true 1
#define false 0


void * wait_answer(){
    int addr_len, numbytes;
    int status;
    struct sockaddr_in client_addr;
    char buf[BUFFER_LEN];
    struct msg *last_message;
    struct sockaddr_in client_address;
    int sockfd;


    /* Server initialization */
    client_address.sin_family      = AF_INET;            
    client_address.sin_port        = htons(RESPONSE_PORT); 
    client_address.sin_addr.s_addr = INADDR_ANY;         
    bzero(&(client_address.sin_zero), 8); 

    /* Socket */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) { perror("socket"); exit(1); }

    /* bind */
    status = bind(sockfd
                 ,(struct sockaddr *)&client_address
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

        /*last_message = (struct msg*) get_writer(cb);
        last_message->in_out = buf[0];
        last_message->car_id = atoi(&buf[1]);
        last_message->client = client_addr.sin_addr.s_addr;
        advance_writer(&cb);*/
        // printf("Escribi\n");
    }

}


int main(int argc, char *argv[])
{
    int sockfd; /* descriptor a usar con el socket */
    struct sockaddr_in their_addr; /* almacenara la direccion IP y numero de puerto del servidor */
    struct hostent *he; /* para obtener nombre del host */
    int numbytes; /* conteo de bytes a escribir */
    int aux = 0;
    int myPort;
    char myOpSerial[4];
    int tries = 0; /* Numero de intentos para conectarse con el servidor */

    if (argc != 9) {
        printf ("Uso: sem_cli -d <nombre_módulo_atención> -p <puerto_sem_svr> -c <op> -i <identificación_vehiculo> \n");
        exit(0);
    }

    /* convertimos el hostname a su direccion IP */
    if      (strcmp("-d",argv[1]) == 0 ) aux = 2;
    else if (strcmp("-d",argv[3]) == 0 ) aux = 4;
    else if (strcmp("-d",argv[5]) == 0 ) aux = 6;
    else if (strcmp("-d",argv[7]) == 0 ) aux = 8;
    else {
        printf("Error: No se ha especificado la direccion de destino.\n");
        exit(0);
    }

    if ((he=gethostbyname(argv[aux])) == NULL) {
        perror("Error: Direccion IP o hostname de destino incorrecto.");
        exit(1);
    }

    /* Verificamos que la operación sea apropiada */
    if      (strcmp("-c",argv[1]) == 0 ) aux = 2;
    else if (strcmp("-c",argv[3]) == 0 ) aux = 4;
    else if (strcmp("-c",argv[5]) == 0 ) aux = 6;
    else if (strcmp("-c",argv[7]) == 0 ) aux = 8;
    else {
        printf("Error: No se ha indicado la operación a realizar.\n");
        exit(0);
    }

    if (strcmp("e",argv[aux])!=0 && strcmp("s",argv[aux])!=0) {
        printf("Error: Operación desconocida.\n");
        exit(1);
    }
    else{
        printf("%s\n", argv[aux]);
        strcpy(myOpSerial, argv[aux]); //copiando la operacion
    }

    /* Verificamos que el puerto este definido en un rango apropiado*/
    if      (strcmp("-p",argv[1]) == 0 ) aux = 2;
    else if (strcmp("-p",argv[3]) == 0 ) aux = 4;
    else if (strcmp("-p",argv[5]) == 0 ) aux = 6;
    else if (strcmp("-p",argv[7]) == 0 ) aux = 8;
    else {
        printf("Error: No se ha especificado el puerto destino.\n");
        exit(0);
    }

    if ((atoi(argv[aux]) > 29999) || (atoi(argv[aux]) < 1024 )) {
        printf("Error: Rango de puerto incorrecto.\n");
        exit(1);
    }
    else{
        myPort=atoi(argv[aux]); //copiando el puerto
    }

    /* Verificamos que el serial sea exista y sea correcto*/
    if      (strcmp("-i",argv[1]) == 0 ) aux = 2;
    else if (strcmp("-i",argv[3]) == 0 ) aux = 4;
    else if (strcmp("-i",argv[5]) == 0 ) aux = 6;
    else if (strcmp("-i",argv[7]) == 0 ) aux = 8;
    else {
        perror("Error: No se ha especificado el serial de identificación.");
        exit(0);
    }

    if (atoi(argv[aux])>200 || atoi(argv[aux])<1  ) {
        perror("Error: Rango de serial (1-200) incorrecto.");
        exit(1);
    }
    else{
        strcat(myOpSerial, argv[aux]); //copiando el serial+1
    }

    /* Creamos el socket */
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        exit(2);
    }


    /* a donde mandar */
    their_addr.sin_family = AF_INET; /* usa host byte order */
    their_addr.sin_port = htons(myPort); /* usa network byte order */
    their_addr.sin_addr = *((struct in_addr *)he->h_addr);
    bzero(&(their_addr.sin_zero), 8); /* pone en cero el resto */
    /* enviamos el mensaje */
    while (tries < 3) {
        numbytes=sendto(sockfd,
                        myOpSerial,
                        strlen(myOpSerial),
                        0,(struct sockaddr *)&their_addr,
                        sizeof(struct sockaddr));

        if ( numbytes == -1) {
            tries++;                /* incrementa el contador y vuelve a intentar */
            fprintf(stderr, "Error al conectarse con el servidor. Intento %d de 3.\n",tries+1);
            //exit(2);
            if (tries==3){
                perror("Tiempo de espera agotado.");
                exit(2);
            }
            else{
                continue;
            }
        }
        printf("enviados %d bytes hacia %s\n--------\n",numbytes,inet_ntoa(their_addr.sin_addr));
        break;
        tries=0;
    }
    /* cierro socket */
    //answerClient(their_addr.sin_addr,"301019941628",0);
    close(sockfd);
    wait_answer();
    exit (0);
}




//----------------------------
//#include <stdio.h>
//#include <stdlib.h>
//#include <errno.h>
//#include <string.h>
//#include <unistd.h>
//#include <netdb.h>
//#include <arpa/inet.h>
//#include <sys/types.h>
//#define SERVER_PORT 4321
//#define BUFFER_LEN 1024
//
//int main(int argc, char *argv[])
//{
    //int sockfd; /* descriptor a usar con el socket */
    //struct sockaddr_in their_addr; /* almacenara la direccion IP y numero de puerto del servidor */
    //struct hostent *he; /* para obtener nombre del host */
    //int numbytes; /* conteo de bytes a escribir */
//    
    //if (argc != 3) {
        //fprintf(stderr,"\nuso: %s cliente hostname mensaje\n", argv[0]);
        //exit(1);
    //}
    ///* convertimos el hostname a su direccion IP */
    //if ((he=gethostbyname(argv[1])) == NULL) {
        //perror("gethostbyname");
        //exit(1);
    //}
    ///* Creamos el socket */
    //if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        //perror("socket");
        //exit(2);
    //}
    ///* a donde mandar */
    //their_addr.sin_family = AF_INET; /* usa host byte order */
    //their_addr.sin_port = htons(SERVER_PORT); /* usa network byte order */
    //their_addr.sin_addr = *((struct in_addr *)he->h_addr);
    //bzero(&(their_addr.sin_zero), 8); /* pone en cero el resto */
    ///* enviamos el mensaje */
    //numbytes=sendto(sockfd,
                    //argv[2],
                    //strlen(argv[2]),
                    //0,(struct sockaddr *)&their_addr,
                    //sizeof(struct sockaddr));
//
    //if ( numbytes == -1) {
        //perror("sendto");
        //exit(2);
    //}
    //printf("enviados %d bytes hacia %s\n",numbytes,inet_ntoa(their_addr.sin_addr));
    ///* cierro socket */
    //close(sockfd);
    //exit (0);
//}