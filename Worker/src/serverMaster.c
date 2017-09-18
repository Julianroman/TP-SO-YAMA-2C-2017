/*
 * ============================================================================
 * serverMaster.c
 * ============================================================================
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

void init_serverMaster(){
	/*
	int socket_listener = crear_listener("8085");
	int socket_cliente  = escuchar_socket(socket_listener,BACKLOG );
	char buffer;
    recv(socket_cliente , buffer, BUFSIZ, 0);
    int file_size = atoi(buffer);


	FILE *received_file = fopen(FILENAME, "w");
    if (received_file == NULL)
    {
            fprintf(stderr, "Failed to open file foo --> %s\n", strerror(errno));

            exit(EXIT_FAILURE);
    }

    int remain_data = file_size;
    int len;

    while (((len = recv(socket_cliente , buffer, BUFSIZ, 0)) > 0) && (remain_data > 0))
    {
            fwrite(buffer, sizeof(char), len, received_file);
            remain_data -= len;
            fprintf(stdout, "Receive %d bytes and we hope :- %d bytes\n", len, remain_data);
    }
    fclose(received_file);

    close(socket_cliente);
    */
};
