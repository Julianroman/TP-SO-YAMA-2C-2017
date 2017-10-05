/*
 * serverWorker.c
 *
 *  Created on: 17/9/2017
 *      Author: utnso
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <sys/socket.h>


#include <commons/collections/dictionary.h>
#include <utilidades/socket_utils.h>
#include <utilidades/protocol/types.h>
#include <utilidades/protocol/receive.h>
#include "serverMaster.h"

#define BACKLOG       5


void init_serverMaster(int puertoEscucha){

	// Diccionario de masters
	//t_dictionary* masters_dictionary = dictionary_create();
	//TODO implementar diccionario de masters


	// Recibir conexion
	int listener = crear_listener(puertoEscucha);
	int backlog  = BACKLOG;
	// Multiplexar listener
	fd_set master,auxfds;
		int i,fdmax,nuevoCliente;
	    socklen_t addrlen;
	    struct sockaddr_storage remoteaddr;
	    HEADER_T header;

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
					} else { // Escuchar mensaje
						void* data = receive(i,&header);
						if (!data){ //Si devuelve NULL es porque se cerro la conexion
							FD_CLR(i,&master); // Eliminar de la lista
							break;
						}
						if(header == SOLICITUDPROCESAMIENTO){  // Estos son errores fantasma
							payload_SOLICITUDPROCESAMIENTO* payload;
							payload = data;
							puts("Mensaje recibido");
							puts(payload->nombreArchivo);

							// IMPORTANTE! HAY UN LEAK DE MEMORIA
							// LA FUNCION DE RECOLECCION ESTA EN DESARROLLO

						}
						// TODO Responder de acuerdo al header recibido
					}
				}
			}
		}
};
