#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
//#define SERVER_PORT 20684
//#define BUFFER_LEN 1024

typedef int bool;
#define true 1
#define false 0

bool answerClient(struct in_addr addr, char info[], bool notFull){

    int socks; /* descriptor a usar con el socket */
    struct sockaddr_in out_addr; /* almacenara la direccion IP y numero de puerto del cliente */
    int sentbytes; /* conteo de bytes a escribir */
    char BoolDateTime[13];

    out_addr.sin_family = AF_INET; /* usa host byte order */
    out_addr.sin_port = htons(20683); /* usa network byte order */
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
    exit (0);
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
    numbytes=sendto(sockfd,
                    myOpSerial,
                    strlen(myOpSerial),
                    0,(struct sockaddr *)&their_addr,
                    sizeof(struct sockaddr));

    if ( numbytes == -1) {
        perror("sendto");
        exit(2);
    }
    printf("enviados %d bytes hacia %s\n",numbytes,inet_ntoa(their_addr.sin_addr));
    /* cierro socket */
    answerClient(their_addr.sin_addr,"301019941628",0);
    close(sockfd);
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