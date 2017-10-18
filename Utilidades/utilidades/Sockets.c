/*
 * Sockets.c
 *
 *  Created on: 11/9/2017
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>
#include <parser/metadata_program.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include <commons/config.h>
#include <commons/string.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <errno.h>
#include "Sockets.h"
t_list* lista;
t_dictionary* diccionario;
const char *vector[20];

char* tipo_proceso(int id_tipo_proceso) {
	switch(id_tipo_proceso) {
		case 0:
			return "File System";
		break;
		case 1:
			return "DataNode";
		break;
		case 2:
			return "Master";
		break;
		default:
			return -1;
		break;
	}
}


void servidor(int puerto){

	fd_set master, masterAux; // conjunto maestro de descriptores de fichero y uno auxiliar para el select()
	FD_ZERO(&master);    // borra los conjuntos maestro y temporal
	FD_ZERO(&masterAux);
	diccionario= dictionary_create();

	// obtener socket a la escucha
	int servidor = socket(AF_INET, SOCK_STREAM, 0);
	if (servidor == -1) {
		perror("Falló el socket");
		exit(1);
	}
	// obviar el mensaje "address already in use" (la dirección ya se está usando)
	int activado = 1;
	int socapete = setsockopt(servidor, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));
	if (socapete == -1) {
		perror("Falló el setsockopt");
		exit(1);
	}

	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = INADDR_ANY;
	direccionServidor.sin_port = htons(puerto);
	memset(&(direccionServidor.sin_zero), '\0', 8);

	if (bind(servidor, (void*) &direccionServidor, sizeof(direccionServidor)) != 0) {
		perror("Falló el bind");
		exit(1);
	}
	// escuchar
	printf("Esperando conexiones...\n");
	if (listen(servidor, 100) == -1) {
		perror("Falló el listen");
		exit(1);
	}

	// añadir servidor al conjunto maestro
	FD_SET(servidor, &master);
	// seguir la pista del descriptor de fichero mayor
	int fdmax = servidor; // por ahora es éste
	struct sockaddr_in direccionCliente; // dirección del cliente
	char* buffer = malloc(1000);
	char* proceso;
	// bucle principal
	while(1) {
		masterAux = master; // cópialo
		if (select(fdmax+1, &masterAux, NULL, NULL, NULL) == -1) {
			perror("Falló el select");
			exit(1);
		}
		// explorar conexiones existentes en busca de datos que leer
		int i;
		for(i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &masterAux)) { // ¡¡tenemos datos!!
				if (i == servidor) {
					// gestionar nuevas conexiones
					int addrlen = sizeof(direccionCliente);
					int cliente = accept(servidor, (void*) &direccionCliente, &addrlen);
					if (cliente  == -1) {
						perror("Falló el accept");
						exit(1);
					} else {
						FD_SET(cliente, &master); // añadir al conjunto maestro
						if (cliente > fdmax) {    // actualizar el máximo
							fdmax = cliente;
						}
						//char* nombreCliente = inet_ntoa(direccionCliente.sin_addr);
						proceso = malloc(16);
						snprintf(proceso, 16, "%d", 0);
						if(dictionary_get(diccionario, proceso) == 0){
							dictionary_put(diccionario, proceso, cliente);
						}
						free(proceso);
						//vector[cliente]= "0";
						char* mensaje = "Bienvenido!!";
						send(cliente, mensaje, strlen(mensaje), 0);
					}
				} else {
					// gestionar datos de un cliente
					proceso = malloc(16);
					snprintf(proceso, 16, "%d", i);
					int id_proceso = dictionary_get(diccionario, proceso);
					//if(vector[i] == "0"){
					if(dictionary_get(diccionario, proceso) == 0){
						int my_net_id;
						int bytesRecibidos = recv(i, &my_net_id, 1000, 0);
						buffer[bytesRecibidos] = '\0';
						int id = ntohl(my_net_id);
						//vector[i] = tipo_proceso(id);
						dictionary_put(diccionario, proceso, id);
						//printf("Recibí una conexión de %s!!\n", vector[i]);
						id_proceso = dictionary_get(diccionario, proceso);
						printf("Recibí una conexión de %s!!\n", tipo_proceso(id_proceso));
						free(proceso);
						}else{
							int bytesRecibidos = recv(i, buffer, 1000, 0);
							if (bytesRecibidos <= 0) {
								// error o conexión cerrada por el cliente
								printf("El socket %s se desconectó\n", tipo_proceso(id_proceso));
								dictionary_remove(diccionario, proceso);
								close(i); // bye!
								FD_CLR(i, &master); // eliminar del conjunto maestro
							} else {
								buffer[bytesRecibidos] = '\0';
								printf("El socket %s dice: %s\n", tipo_proceso(id_proceso), buffer);
							}
							free(proceso);
						}
					} // Esto es ¡TAN FEO!
				}
			}
		}
		free(buffer);
}

void cliente(const char* ip, int puerto, int id_tipo_proceso){
	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = INADDR_ANY;
	//direccionServidor.sin_addr.s_addr = inet_addr(ip);
	direccionServidor.sin_port = htons(puerto);

	int cliente = socket(AF_INET, SOCK_STREAM, 0);
	if (connect(cliente, (void*) &direccionServidor, sizeof(direccionServidor)) != 0) {
		perror("No se pudo conectar");
		exit(1);
	}

	int numeroConvertido = htonl(id_tipo_proceso);
	send(cliente, &numeroConvertido, sizeof(numeroConvertido), 0);

	//------- Mensaje de bienvenida del FileSystem ---------------
	//char buf[256];
	char* buffer = malloc(1000);
	int bytesRecibidos = recv(cliente, buffer, 1000, 0);
	buffer[bytesRecibidos] = '\0';
	printf("FileSystem dice: %s\n", cliente, buffer);
	//------------------------------------------------------------

	while (1) {
		char mensaje[1000];
		scanf("%s", mensaje);
		send(cliente, mensaje, strlen(mensaje), 0);
	}
	free(buffer);
}

void enviar_archivo(int socketCliente, char* file){
	socklen_t       sock_len;
	ssize_t len;
	int fd;
	int sent_bytes = 0;
	char file_size[256];
	struct stat file_stat;
	int offset;
	int remain_data;

	fd = fopen(file, "r");
	if (fd == -1)
	{
			fprintf(stderr, "Error opening file --> %s", strerror(errno));

			exit(EXIT_FAILURE);
	}

	/* Get file stats */
	if (fstat(fd, &file_stat) < 0)
	{
			fprintf(stderr, "Error fstat --> %s", strerror(errno));

			exit(EXIT_FAILURE);
	}
	/* Sending file size */
	len = send(socketCliente, file_size, sizeof(file_size), 0);
	if (len < 0)
	{
		fprintf(stderr, "Error on sending greetings --> %s", strerror(errno));

		exit(EXIT_FAILURE);
	}

	fprintf(stdout, "Server envió %d bytes para el tamaño\n", len);

	offset = 0;
	remain_data = file_stat.st_size;
	/* Sending file data */
	while (((sent_bytes = sendfile(socketCliente, fd, &offset, BUFSIZ)) > 0) && (remain_data > 0))
	{
			fprintf(stdout, "1. Server sent %d bytes from file's data, offset is now : %d and remaining data = %d\n", sent_bytes, offset, remain_data);
			remain_data -= sent_bytes;
			fprintf(stdout, "2. Server sent %d bytes from file's data, offset is now : %d and remaining data = %d\n", sent_bytes, offset, remain_data);
	}
}

void recibir_archivo(int socketCliente){
	int client_socket;
	ssize_t len;
	struct sockaddr_in remote_addr;
	char buffer[BUFSIZ];
	int file_size;
	FILE *received_file;
	int remain_data = 0;

    /* Receiving file size */
    recv(socketCliente, buffer, BUFSIZ, 0);
    file_size = atoi(buffer);
    //fprintf(stdout, "\nFile size : %d\n", file_size);

    received_file = fopen("archivoRecibido.c", "w");
    if (received_file == NULL)
    {
            fprintf(stderr, "Failed to open file foo --> %s\n", strerror(errno));

            exit(EXIT_FAILURE);
    }

    remain_data = file_size;

    while (((len = recv(client_socket, buffer, BUFSIZ, 0)) > 0) && (remain_data > 0))
    {
            fwrite(buffer, sizeof(char), len, received_file);
            remain_data -= len;
            fprintf(stdout, "Receive %d bytes and we hope :- %d bytes\n", len, remain_data);
    }
    fclose(received_file);
}
