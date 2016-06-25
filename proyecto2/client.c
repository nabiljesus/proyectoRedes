#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#define SERVER_PORT 4321
#define BUFFER_LEN 1024

int main(int argc, char *argv[])
{
    int sockfd; /* descriptor a usar con el socket */
    struct sockaddr_in their_addr;; /* almacenara la direccion IP y numero de puerto del servidor */
    struct hostent *he; /* para obtener nombre del host */
    int numbytes; /* conteo de bytes a escribir */
    int aux = 0;
    char ope[1];
    char myPort[5];
    char mySerial[3];

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
        perror("Error: No se ha especificado la direccion de destino.");
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
        perror("Error: No se ha indicado la operación a realizar.");
        exit(0);
    }

    if (strcmp("e",argv[aux])!=0 && strcmp("s",argv[aux])!=0) {
        perror("Error: Operación desconocida.");
        exit(1);
    }
    else{
        strcpy(ope, argv[aux]); //copiando la operacion
    }

    /* Verificamos que el puerto este definido en un rango apropiado*/
    if      (strcmp("-p",argv[1]) == 0 ) aux = 2;
    else if (strcmp("-p",argv[3]) == 0 ) aux = 4;
    else if (strcmp("-p",argv[5]) == 0 ) aux = 6;
    else if (strcmp("-p",argv[7]) == 0 ) aux = 8;
    else {
        perror("Error: No se ha especificado el puerto destino.");
        exit(0);
    }

    if (atoi(argv[aux])>29999 || atoi(argv[aux])<20000  ) {
        perror("Error: Rango de puerto incorrecto.");
        exit(1);
    }
    else{
        strcpy(myPort, argv[aux]); //copiando el puerto
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
        strcpy(mySerial, argv[aux]); //copiando el serial+1
    }

    /* Creamos el socket */
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        exit(2);
    }


    /* a donde mandar */
    their_addr.sin_family = AF_INET; /* usa host byte order */
    their_addr.sin_port = htons(SERVER_PORT); /* usa network byte order */
    their_addr.sin_addr = *((struct in_addr *)he->h_addr);
    bzero(&(their_addr.sin_zero), 8); /* pone en cero el resto */
    /* enviamos el mensaje */
    numbytes=sendto(sockfd,
                    argv[2],
                    strlen(argv[2]),
                    0,(struct sockaddr *)&their_addr,
                    sizeof(struct sockaddr));

    if ( numbytes == -1) {
        perror("sendto");
        exit(2);
    }
    printf("enviados %d bytes hacia %s\n",numbytes,inet_ntoa(their_addr.sin_addr));
    /* cierro socket */
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