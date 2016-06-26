#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#define BUFFER_LEN 1024
#define MACHINEID  3

typedef int bool;
#define true 1
#define false 0

void printTicket(char * info){
    char * ticket[15];
    int i;
    
    char ticket12[60];
    char aux[60];
    char aux2[60];
    system("cls");
    system("clear");

    for (i=0;i<3;i++)
        printf("\n");
    ticket[0]="   ############################################################";
    ticket[1]="   #      ____   ____     __  __            _       _         #";
    ticket[2]="   #     / ___| / ___|   |  \\/  | ___  _ __(_) __ _| |__      #";
    ticket[3]="   #    | |    | |       | |\\/| |/ _ \\| '__| |/ _` | '_ \\     #";
    ticket[4]="   #    | |___ | |___ _  | |  | | (_) | |  | | (_| | | | |    #";
    ticket[5]="   #     \\____(_)____(_) |_|  |_|\\___/|_|  |_|\\__,_|_| |_|    #";
    ticket[6]="   #                                                          #";
    ticket[7]="   #              Ticket de estacionamiento                   #";
    ticket[8]="   # La reposición de este ticket tiene un costo de BsF.1.500 #";
    ticket[9]="   #                                                          #";
    ticket[10]="   #              Fecha: DD/MM/YY    Hora: HH/mm              #";
    ticket[11]="   #                       Serial:  SSS                       #";
    sprintf(ticket12,"   #                       Puerta:  %d                         #",MACHINEID);
    ticket[12]=ticket12;
    ticket[13]="   #                                                          #";
    ticket[14]="   ############################################################";

    if (info[0]=='0'){    
        /* Impresion del caso lleno*/
        ticket[7]="   #                                                          #";
        ticket[8]="   #                                                          #";
        ticket[10]="   #                NO HAY PUESTOS DISPONIBLES                #";
        ticket[11]="   #                                                          #";

        for (i=0; i<15; i++){
            printf("%s\n",ticket[i]);
        }
    }
    else if (info[0]=='1'){
        /* Impresion del caso con puesto */
        char DD[3];
        char MM[3];
        char YYYY[5];
        char HH[3];
        char mm[3];
        char sss[4];
        //BDDMMYYYYHHMMSSS
        DD[2]=0;
        MM[2]=0;
        YYYY[4]=0;
        HH[2]=0;
        mm[2]=0;
        sss[3]=0;
        strncpy(DD, info+1, 2);
        strncpy(MM, info+3, 2);
        strncpy(YYYY, info+5, 4);
        strncpy(HH, info+9, 2);
        strncpy(mm, info+11, 2);
        strncpy(sss, info+13, 3);
        sprintf(aux, "   #              Fecha: %s/%s/%s  Hora: %s/%s              #", DD,MM,YYYY,HH,mm);
        ticket[10]=aux;
        sprintf(aux2,"   #                       Serial:  %s                       #", sss);
        ticket[11]=aux2;
        for (i=0; i<15; i++){
            printf("%s\n",ticket[i]);
        }
        ;
    }
    else{
        /* Impresion del costo */
        ticket[7]="   #                                                          #";
        ticket[8]="   #                                                          #";
        sprintf(aux,"   #                TOTAL A PAGAR: %s,00 BsF      ",info);
        for (i=0;i<14-strlen(info);i++)
            strcat(aux," ");
        strcat(aux,"#");

        ticket[10]=aux;
        ticket[11]="   #                                                          #";
        for (i=0; i<15; i++){
            printf("%s\n",ticket[i]);
        }
    }

    for (i=0;i<3;i++)
        printf("\n");


}


bool  wait_answer(char * buf){
    /* Procedimiento que espera por una respuesta del servidor */
    int addr_len, numbytes;
    int status;
    struct sockaddr_in client_addr;
    struct msg *last_message;
    struct sockaddr_in client_address;
    int sockfd2;
    struct timeval tv;
    // Tiempo máximo de espera por una respuesta. (Hasta 3 veces).
    tv.tv_sec = 30;  /* 30 Secs Timeout */
    tv.tv_usec = 0;  

    /* Parametrización del socket */
    client_address.sin_family      = AF_INET;            
    client_address.sin_port        = htons(20683); 
    client_address.sin_addr.s_addr = INADDR_ANY;         
    bzero(&(client_address.sin_zero), 8); 

    /* Creando el socket */
    sockfd2 = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd2 == -1) { perror("socket"); exit(1); }
    setsockopt(sockfd2, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,sizeof(struct timeval));
    /* haciendo bind */
    status = bind(sockfd2
                 ,(struct sockaddr *)&client_address
                 ,sizeof(struct sockaddr));
    if (status == -1)  { perror("bind"); exit(2); }
    
    addr_len = sizeof(struct sockaddr);

    /* Esperando por los datos */
    //printf("Esperando datos ....\n");
    numbytes = recvfrom(sockfd2, buf, 
                        BUFFER_LEN, 0, 
                        (struct sockaddr *)&client_addr,
                        (socklen_t *)&addr_len);

    if ( numbytes <= 0 ) {
        perror("Error: Sin respuesta del servidor.");
    }
    else {
        //printf("paquete proveniente de : %s\n",inet_ntoa(client_addr.sin_addr));
        //printf("longitud del paquete en bytes: %d\n",numbytes);
        buf[numbytes] = '\0';
        //printf("el paquete contiene: %s\n", buf);      //Aqui se debería imprimir.
    }
    close(sockfd2);
    return (numbytes > 0);

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
    char * answer[16];

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

    if (atoi(argv[aux])>199 || atoi(argv[aux])<0  ) {
        perror("Error: Rango de serial (0-199) incorrecto.");
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
    while (tries < 3) { //En caso de que el servidor este apagdo, intenta a lo sumo 3 veces.
        numbytes=sendto(sockfd,
                        myOpSerial,
                        strlen(myOpSerial),
                        0,(struct sockaddr *)&their_addr,
                        sizeof(struct sockaddr));

//        printf("enviados %d bytes hacia %s\n--------\n",numbytes,inet_ntoa(their_addr.sin_addr));
        if ( numbytes < 0) {
            perror("Error de conexion.");
        }

        if(!(wait_answer(answer))){
            tries++;                /* incrementa el contador y vuelve a intentar */
            fprintf(stderr, "Error al conectarse con el servidor. Intento %d de 3.\n",tries);
            //exit(2);
            if (tries==3){
                perror("Tiempo de espera agotado.");
                exit(2);
            }
            else{
                continue;
            }
            tries=0;

        }
        else{
            printTicket(answer);
            close(sockfd);
            exit (0);
        }
    }
    /* cierro socket */
    //answerClient(their_addr.sin_addr,"301019941628",0);
    close(sockfd);
    exit (0);
}

