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

// #define SERVER_PORT 4321
#define BUFFER_LEN 1024

#define PARKING_LOT_SIZE 200
#define HOUR_PRICE 80
#define HOUR_FRACTION_PRICE 30

#define WENT_IN  1
#define WENT_OUT 0

// Store hours in array
struct tm * parking_space[PARKING_LOT_SIZE];



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


void write_action(char *output_file,int coming_inside,int door,int i){
    char action[8];
    char time_buffer[30];
    FILE *log_file = fopen(output_file, "a");

    // Check there is actually a car in there
    assert(parking_space[i] != NULL);

    if (coming_inside) strcpy(action,"entrado");
    else        strcpy(action,"salido");

    strftime(time_buffer, 30, "%d/%m/%Y %H:%M:%S", parking_space[i]);

    fprintf(log_file
           ,"%s | carro %d ha %s por la puerta %d \n"
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

void read_message(int socket){
    int addr_len, numbytes;
    struct sockaddr_in client_addr;
    char buf[BUFFER_LEN];

    addr_len = sizeof(struct sockaddr);
    printf("Esperando datos ....\n");
    numbytes = recvfrom(socket, buf, 
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
}

/*void send_ticket(int socket){

}*/


int main(int argc, char *argv[])
{
    int free_parking_lots = PARKING_LOT_SIZE;
    int listen_port;              // Port we will be listening
    char *entrance_log,*exit_log; // Strings for log files
    
    int status;      // Auxiliary to check procedure returning values
    int i;
    int sockfd;
    int last_ticket;
    
    
    struct sockaddr_in server_address;

    assert(argc == 7);

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


    /* Server initialization */
    server_address.sin_family      = AF_INET;            
    server_address.sin_port        = htons(listen_port); 
    server_address.sin_addr.s_addr = INADDR_ANY;         
    bzero(&(server_address.sin_zero), 8); 

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd == -1) { perror("socket"); exit(1); }

    status = bind(sockfd
                 ,(struct sockaddr *)&server_address
                 ,sizeof(struct sockaddr));

    if (status == -1)  { perror("bind"); exit(2); }


    


    // Estados
    while (1){
        // Entregar tickets y aceptar tickets de salida
        while(free_parking_lots > 0 ) {

            read_message(sockfd); //&last_ticket // Entregar puerta, ticket y si es entrada o salida
            // Si alguien entra
            if (1)
            {   
                last_ticket = new_ticket();
                write_action(entrance_log,WENT_IN,1,last_ticket);

                --free_parking_lots;
            }
            else
            {
                ++free_parking_lots;
                // Se debe crear un ticket con los datos recibdos
                write_action(exit_log,WENT_OUT,1,last_ticket);
                
            }

            
        }
        
        // Rebotar gente hasta encontrar algun ticket de salida
        while(free_parking_lots == 0)
        {
            read_message(sockfd);//&last_ticket

            ++free_parking_lots;
            // Se debe crear un ticket con los datos recibdos
            write_action(exit_log,WENT_OUT,1,last_ticket);
        }
        sleep(3);
    }

}
