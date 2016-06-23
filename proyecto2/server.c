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
    int sockfd; /* descriptor para el socket */
    struct sockaddr_in my_addr; /* direccion IP y numero de puerto local */ 
    struct sockaddr_in their_addr; /* direccion IP y numero de puerto del cliente */
    /* addr_len contendra el taman~o de la estructura sockadd_in y numbytes el
    * numero de bytes recibidos */
    int addr_len, numbytes;
    int status;
    char buf[BUFFER_LEN]; /* Buffer de recepción */
    /* se crea el socket */

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    /* Se establece la estructura my_addr para luego llamar a bind() */
    my_addr.sin_family      = AF_INET; /* usa host byte order */
    my_addr.sin_port        = htons(SERVER_PORT); /* usa network byte order */
    my_addr.sin_addr.s_addr = INADDR_ANY; /* escuchamos en todas las IPs */
    bzero(&(my_addr.sin_zero), 8); /* rellena con ceros el resto de la estructura */

    /* Se le da un nombre al socket (se lo asocia al puerto e IPs) */
    printf("Asignado direccion al socket ....\n");
    status = bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr));
    
    if (status == -1) {
        perror("bind");
        exit(2);
    }

    /* Se reciben los datos (directamente, UDP no necesita conexión) */
    addr_len = sizeof(struct sockaddr);
    printf("Esperando datos ....\n");
    numbytes = recvfrom(sockfd, buf, 
                        BUFFER_LEN, 0, 
                        (struct sockaddr *)&their_addr,
                        (socklen_t *)&addr_len)
    if ( numbytes == -1) {
        perror("recvfrom");
        exit(3);
    }
    /* Se visualiza lo recibido */
    printf("paquete proveniente de : %s\n",inet_ntoa(their_addr.sin_addr));
    printf("longitud del paquete en bytes: %d\n",numbytes);
    buf[numbytes] = '\0';
    printf("el paquete contiene: %s\n", buf);
    /* cerramos descriptor del socket */
    close(sockfd);
    exit (0);
}