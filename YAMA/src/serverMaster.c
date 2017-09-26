/*
 * serverWorker.c
 *
 *  Created on: 17/9/2017
 *      Author: utnso
 */
#include <utilidades/socket_utils.h>
#include <utilidades/protocolo.h>
#include <utilidades/protocol_utils.h>
#include "responses.h"
#include "serverMaster.h"

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
#include <pthread.h>
#include <errno.h>

#define PUERTOESCUCHA "8085"
#define BACKLOG       5

void multiplexar(int listener,int backlog,YAMA_STATUS (*respuesta)(t_Mensaje,int));

void init_serverMaster(int puertoEscucha){
	// Recibir conexion
	int socket_listener = crear_listener(puertoEscucha);
	// Multiplexar listener
	multiplexar(socket_listener,BACKLOG,&responder_solicitud);
};

void multiplexar(int listener,int backlog,YAMA_STATUS (*respuesta)(t_Mensaje,int)){
	fd_set master,auxfds;
	int i,fdmax,nuevoCliente,nbytes;
    socklen_t addrlen;
    struct sockaddr_storage remoteaddr;
    t_Mensaje header;

	//Escuchar listener
	if(listen(listener,backlog)==-1){
		perror("Listen");
		exit(EXIT_FAILURE);
	}
	// Limpiar los sets de fd
	FD_ZERO(&master);
	FD_ZERO(&auxfds);
	//Agregar listener a la lista
	FD_SET(listener,&master);
	fdmax = listener;

	// Loop principal
	while(1){
		auxfds = master;
		if(select(fdmax+1, &auxfds, NULL, NULL, NULL)==-1){
			perror("Select");
			exit(EXIT_FAILURE);
		}

		for(i = 0; i<=fdmax;i++){
			if (FD_ISSET(i, &auxfds)) {
				if (i == listener) {
					// Nueva conexion
					addrlen = sizeof remoteaddr;
					nuevoCliente = accept(listener,(struct sockaddr *)&remoteaddr,&addrlen);
					if (nuevoCliente == -1) {
						perror("accept");
					} else {
						FD_SET(nuevoCliente, &master); // Agregar al set master
						if (nuevoCliente > fdmax) {
							fdmax = nuevoCliente;
						}
					}
				} else {
					// Recibir peticion desde cliente (header)
					if ((nbytes = recv(i, &header, sizeof header, 0)) <= 0) {
						// Conexion cerrada
						if (nbytes == 0) {
							printf("socket %d cerro la conexion\n", i);
						} else {
							perror("recv");
						}
						close(i); 			// cerrar socket
						FD_CLR(i, &master); // removerlo del master
					} else {
						// Responder
						(*respuesta)(header,i);
					}
				}
			}
		}
	}
}
