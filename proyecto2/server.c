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

// #define SERVER_PORT 4321
#define BUFFER_LEN 1024

#define PARKING_LOT_SIZE 200
#define HOUR_PRICE 80
#define HOUR_FRACTION_PRICE 30

#define WENT_IN  1
#define WENT_OUT 0

/* Ticket structure*/
struct ticket
{
    int       identifier; /* Unique ticket/car identifier */
    struct tm *enter_time; /* Time when car checked-in     */
};

void update_ticket(struct ticket *t,int newval){
    time_t time_data;
    time(&time_data);

    t->identifier = newval;
    t->enter_time = localtime(&time_data);
}


void write_action(char *output_file,int inside,int door,struct ticket t){
    char action[8];
    char time_buffer[30];
    FILE *log_file = fopen(output_file, "a");

    if (inside) strcpy(action,"entrado");
    else        strcpy(action,"salido");

    strftime(time_buffer, 30, "%d/%m/%Y %H:%M:%S", t.enter_time);

    fprintf(log_file
           ,"%s | carro %d ha %s por la puerta %d \n"
           ,time_buffer
           ,t.identifier
           ,action
           ,door);

    fclose(log_file); 
}



int main(int argc, char *argv[])
{
    int ticket_number     = 0;
    int free_parking_lots = PARKING_LOT_SIZE;
    int listen_port; // Port we will be listening
    char *entrance_log,*exit_log;
    
    int status;      // Auxiliary to check procedure returning values

    int sockfd;
    struct sockaddr_in server_address;
    struct ticket last_ticket;


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


    /* Server initialization */
    // server_address.sin_family      = AF_INET;            
    // server_address.sin_port        = htons(listen_port); 
    // server_address.sin_addr.s_addr = INADDR_ANY;         
    // bzero(&(server_address.sin_zero), 8); 

    // sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    // if ((sockfd) == -1) {
    //     perror("socket");
    //     exit(1);
    // }

    // status = bind(sockfd, (struct sockaddr *)&server_address, sizeof(struct sockaddr));

    // if (status == -1) {
    //     perror("Error");
    //     exit(2);
    // }

    // Estados
    while (1){
        // Entregar tickets y aceptar tickets de salida
        while(free_parking_lots > 0 ) {


            // Si alguien entra
            if (1)
            {   
                update_ticket(&last_ticket,ticket_number);
                write_action(entrance_log,WENT_IN,1,last_ticket);

                ++ticket_number;
                --free_parking_lots;
            }
            else
            {
                ++free_parking_lots;
                // Se debe crear un ticket con los datos recibdos
                write_action(exit_log,WENT_OUT,1,last_ticket);
                
            }

            
        }
        sleep(3);
        // Rebotar gente hasta encontrar algun ticket de salida
        while(free_parking_lots == 0)
        {
            ++free_parking_lots;
            // Se debe crear un ticket con los datos recibdos
            write_action(exit_log,WENT_OUT,1,last_ticket);
        }
        sleep(3);
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