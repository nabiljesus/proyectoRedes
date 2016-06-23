#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>

#include <time.h>
#include <string.h>

#define SERVER_PORT 4321
#define BUFFER_LEN 1024

#define PARKING_LOT_SIZE 200
#define HOUR_PRICE 80
#define HOUR_FRACTION_PRICE 30

/* Ticket structure*/
struct ticket
{
    int       identifier; /* Unique ticket/car identifier */
    struct tm enter_time; /* Time when car checked-in     */
};
 

int main(int argc, char *argv[])
{

    int ticket_number     = 0;
    int free_parking_lots = PARKING_LOT_SIZE;
    int listen_port;

    int sockfd;
    struct sockaddr_in server_address;


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

    /* Server initialization */
    server_address.sin_family      = AF_INET;            /* usa host byte order */
    server_address.sin_port        = htons(listen_port); /* usa network byte order */
    server_address.sin_addr.s_addr = INADDR_ANY;         /* escuchamos en todas las IPs */
    bzero(&(server_address.sin_zero), 8); /* rellena con ceros el resto de la estructura */

    // Estados
    while (1){
        while(free_parking_lots > 0 ) ; // Entregar tickets y aceptar tickets de salida
        while(free_parking_lots == 0) ; // Rebotar gente hasta encontrar algun ticket de salida
    }

}

// Taller funcional, esto es todo el main
//     int sockfd; /* descriptor para el socket */
//     struct sockaddr_in my_addr; /* direccion IP y numero de puerto local */ 
//     struct sockaddr_in their_addr; /* direccion IP y numero de puerto del cliente */
//     /* addr_len contendra el taman~o de la estructura sockadd_in y numbytes el
//     * numero de bytes recibidos */
//     int addr_len, numbytes;
//     int status;
//     char buf[BUFFER_LEN]; /* Buffer de recepción */
//     /* se crea el socket */

//     if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
//         perror("socket");
//         exit(1);
//     }

//     /* Se establece la estructura my_addr para luego llamar a bind() */
//     my_addr.sin_family      = AF_INET; /* usa host byte order */
//     my_addr.sin_port        = htons(SERVER_PORT); /* usa network byte order */
//     my_addr.sin_addr.s_addr = INADDR_ANY; /* escuchamos en todas las IPs */
//     bzero(&(my_addr.sin_zero), 8); /* rellena con ceros el resto de la estructura */

//     /* Se le da un nombre al socket (se lo asocia al puerto e IPs) */
//     printf("Asignado direccion al socket ....\n");
//     status = bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr));
    
//     if (status == -1) {
//         perror("bind");
//         exit(2);
//     }

//     /* Se reciben los datos (directamente, UDP no necesita conexión) */
//     addr_len = sizeof(struct sockaddr);
//     printf("Esperando datos ....\n");
//     numbytes = recvfrom(sockfd, buf, 
//                         BUFFER_LEN, 0, 
//                         (struct sockaddr *)&their_addr,
//                         (socklen_t *)&addr_len);

//     if ( numbytes == -1 ) {
//         perror("recvfrom");
//         exit(3);
//     }
//     /* Se visualiza lo recibido */
//     printf("paquete proveniente de : %s\n",inet_ntoa(their_addr.sin_addr));
//     printf("longitud del paquete en bytes: %d\n",numbytes);
//     buf[numbytes] = '\0';
//     printf("el paquete contiene: %s\n", buf);
//     /* cerramos descriptor del socket */
//     close(sockfd);
//     exit (0);
// }