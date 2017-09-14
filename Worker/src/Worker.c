/*
 ============================================================================
 Name        : Worker.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <parser/metadata_program.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <commons/string.h>
#include <string.h>
#include <unistd.h>
#include <utilidades/socket_utils.h>

/*
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>


#define FILENAME        "/home/toc/foo.c"

#define BACKLOG 5
*/

t_log* log;
int main(void) {
	puts("Comienza Worker");

	// Manejo de logs
	log = log_create("worker.log", "Worker", false, LOG_LEVEL_TRACE);
	//log_trace(log, "MENSAJE");

	// Servidor
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

	return EXIT_SUCCESS;
}
