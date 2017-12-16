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

int idUltimoMasterCreado = 1;
#define BACKLOG       5

void agregarAListado(int socket){
	t_job_master* job_master = malloc(sizeof(t_job_master));
	job_master->master_id = idUltimoMasterCreado;
	job_master->master_socket = socket;
	list_add(MastersJobs, job_master);
	send_JOB(socket, idUltimoMasterCreado);
	idUltimoMasterCreado ++;
}

void eliminarMaster(int masterID){
	int eliminar(t_job_master* job_master){
		return job_master->master_id == masterID;
	}
	list_remove_by_condition(MastersJobs, (void*)eliminar);
}

int getMasterID(int socketMaster){

	if(list_size(MastersJobs) > 0){
		int buscar(t_job_master* job_master){
			return job_master->master_socket == socketMaster;
		}
		t_job_master* master = list_find(MastersJobs, (void*)buscar);
		return master->master_id;
	}

	return -1;
}

void init_serverYAMA(int puertoEscucha){

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
							log_trace(logYAMA, "Se conecto Master %d", idUltimoMasterCreado);
							agregarAListado(nuevoCliente);
						}
					} else { // Escuchar mensaje
						void* data = receive(i,&header);
						if (header == FIN_COMUNICACION){ //Si header es FIN_COMUNICACION es porque se cerro la conexion

							int masterID = getMasterID(i);
							if(masterID == -1){
								log_error(logYAMA, "Master Inexistente");
							}
							else{
								eliminarMaster(masterID);
								log_error(logYAMA, "Se desconecto Master %d", masterID);
							}
							FD_CLR(i,&master); // Eliminar de la lista
							close(i);
						}
						else{
							responder_SOLICITUD(i, data, header); // Responder solicitud
						}
					}
				}
			}
		}
};
