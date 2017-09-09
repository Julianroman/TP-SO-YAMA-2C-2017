/*
 * Sockets.h
 *
 *  Created on: 4/6/2017
 *      Author: utnso
 */

#ifndef SOCKETS_H_
#define SOCKETS_H_

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

void servidor(int puerto){
	
	fd_set master, masterAux; // conjunto maestro de descriptores de fichero y uno auxiliar para el select()
	FD_ZERO(&master);    // borra los conjuntos maestro y temporal
	FD_ZERO(&masterAux);

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
	// bucle principal
	while(1) {
		masterAux = master; // cópialo
		if (select(fdmax+1, &masterAux, NULL, NULL, NULL) == -1) {
			perror("Falló el select");
			exit(1);
		}
		// explorar conexiones existentes en busca de datos que leer
		int i, j;
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
						printf("Recibí una conexión de %d!!\n", cliente);
						char* mensaje = "Bienvenido al FileSystem!!";
						send(cliente, mensaje, strlen(mensaje), 0);
					}
				} else {
					// gestionar datos de un cliente
					//char buf[256];
					int bytesRecibidos = recv(i, buffer, 1000, 0);
					if (bytesRecibidos <= 0) {
						// error o conexión cerrada por el cliente
						printf("El socket %d se desconectó\n", i);
						close(i); // bye!
						FD_CLR(i, &master); // eliminar del conjunto maestro
						} else {
							buffer[bytesRecibidos] = '\0';
							printf("El socket %d dice: %s\n", i, buffer);
							if (FD_ISSET(j, &master)) {
								// excepto al listener y a nosotros mismos
	                        	/*	if (j != servidor && j != i) {
	                        			if (send(j, buffer, bytesRecibidos, 0) == -1) {
	                        				perror("send");
	                        				exit(1);
	                        			}
	                        		}*/
							}
						}
				} // Esto es ¡TAN FEO!
			}
		}
	}
	free(buffer);
}

void cliente(const char* ip, int puerto){
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

	//------- Mensaje de bienvenida del FileSystem ---------------
	//char buf[256];
	char* buffer = malloc(1000);
	int bytesRecibidos = recv(cliente, buffer, 1000, 0);
	buffer[bytesRecibidos] = '\0';
	printf("%d dice: %s\n", cliente, buffer);
	//------------------------------------------------------------

	while (1) {
		char mensaje[1000];
		scanf("%s", mensaje);
		send(cliente, mensaje, strlen(mensaje), 0);
	}

	free(buffer);

}

#endif /* SOCKETS_H_ */
