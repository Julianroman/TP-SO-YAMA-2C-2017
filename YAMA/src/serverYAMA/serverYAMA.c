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
#include <commons/log.h>
#include <utilidades/socket_utils.h>
#include <utilidades/protocol/types.h>
#include <utilidades/protocol/receive.h>
#include <utilidades/protocol/senders.h>
#include "responses.h"
#include "serverYAMA.h"
#include "../YAMA.h"

extern t_log* logs;

#define BACKLOG       5


void init_serverYAMA(int puertoEscucha){

	// Variable de estado que devuelven las respuestas a solicitudes
	YAMA_STATUS status;

	int* idUltimoMasterCreado = 1;

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
						}
						else {
							FD_SET(nuevoCliente, &master); // Agregar al set master
							if (nuevoCliente > fdmax) {
								fdmax = nuevoCliente;
								}
							agregarADiccionarioMaster(idUltimoMasterCreado, i);
							*idUltimoMasterCreado += 1;
						}
					} else { // Escuchar mensaje
						void* data = receive(i,&header);
						if (header == FIN_COMUNICACION){ //Si header es FIN_COMUNICACION es porque se cerro la conexion
							FD_CLR(i,&master); // Eliminar de la lista
							break;
						}
						else{
							status = responder_SOLICITUD(i, data, header); // Responder solicitud
							if (status == EXITO){
								log_trace(logs, "Solicitud respondida con exito");
							}
							else if(status == ERROR){
								log_error(logs, "Hubo un error en la solicitud");
								break;
							}
						}
					}
				}
			}
		}
};
