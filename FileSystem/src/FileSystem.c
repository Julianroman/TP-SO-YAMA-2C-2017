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
	        direccionServidor.sin_port = htons(5040);
	        memset(&(direccionServidor.sin_zero), '\0', 8);

	        if (bind(servidor, (void*) &direccionServidor, sizeof(direccionServidor)) != 0) {
	        	perror("Falló el bind");
	        	return 1;
	        }
	        // escuchar
	        printf("Estoy escuchando\n");
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
	                        } else {
	                            FD_SET(cliente, &master); // añadir al conjunto maestro
	                            if (cliente > fdmax) {    // actualizar el máximo
	                                fdmax = cliente;
	                            }
	                            //char* nombreCliente = inet_ntoa(direccionCliente.sin_addr);
	                            printf("Recibí una conexión de %d!!\n", cliente);
	                        }
	                    } else {
	                        // gestionar datos de un cliente
	                    	char buf[256];
	                    	int bytesRecibidos = recv(i, buffer, sizeof(buf), 0);
	                        if (bytesRecibidos <= 0) {
	                            // error o conexión cerrada por el cliente
	                        	printf("FileSystem: el socket %d se desconectó\n", i);
	                            close(i); // bye!
	                            FD_CLR(i, &master); // eliminar del conjunto maestro
	                        } else {
	                        	buffer[bytesRecibidos] = '\0';
	                        	printf("El socket %d dice: %s\n", i, buffer);
	                        	if (FD_ISSET(j, &master)) {
	                        	    // excepto al listener y a nosotros mismos
	                        		if (j != servidor && j != i) {
	                        			if (send(j, buffer, bytesRecibidos, 0) == -1) {
	                        				perror("send");
	                        			}
	                        	   }
	                        	}
	                        }
	                    } // Esto es ¡TAN FEO!
	                }
	            }
	        }

	        return EXIT_SUCCESS;
}
