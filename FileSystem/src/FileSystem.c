/*
 ============================================================================
 Name        : FileSystem.c
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
#include <stdint.h>
#include <arpa/inet.h>
#include <sys/socket.h>

t_log* log;

int main(void) {
	puts("Comienza el proceso FileSystem");
	log = log_create("fileSystem.log", "FileSystem", false, LOG_LEVEL_TRACE);
	//log_trace(log, "MENSAJE");

	struct sockaddr_in direccionServidor;
		direccionServidor.sin_family = AF_INET;
		direccionServidor.sin_addr.s_addr = inet_addr("127.0.0.1");
		direccionServidor.sin_port = htons(5040);

		int servidor = socket(AF_INET, SOCK_STREAM, 0);

		int activado = 1;
		setsockopt(servidor, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));

		if (bind(servidor, (void*) &direccionServidor, sizeof(direccionServidor)) != 0) {
			perror("Fallo el bind");
			return 1;
		}

		printf("Estoy escuchando\n");
		listen(servidor, 100);

		//------------------------------

		struct sockaddr_in direccionCliente;
		unsigned int tamanoDireccion;
		int cliente = accept(servidor, (void*) &direccionCliente, &tamanoDireccion);

		char* nombreCliente = inet_ntoa(direccionCliente.sin_addr);
		printf("Recibí una conexion de %s!!\n", nombreCliente);
		send(cliente, "Hola NetCat!", 13, 0);
		send(cliente, ":)\n", 4, 0);

		//------------------------------

		char* buffer = malloc(1000);

		while (1) {
			int bytesRecibidos = recv(cliente, buffer, 1000, 0);
			if (bytesRecibidos <= 0) {
				printf("Se desconectó %s.\n", nombreCliente);
				//perror("El chabon se desconecto o bla.");
				return 1;
			}

			buffer[bytesRecibidos] = '\0';
			printf("%s dice: %s\n", nombreCliente, buffer);
		}

		free(buffer);

	return EXIT_SUCCESS;
}
