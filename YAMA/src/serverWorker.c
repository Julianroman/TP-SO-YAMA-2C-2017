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
#include "serverWorker.h"

#define PUERTOESCUCHA "8085"
#define BACKLOG       5



void init_WorkerServer(){
	//Recibir conexion
	int socket_listener = crear_listener(PUERTOESCUCHA);
	int socket_cliente = escuchar_socket(socket_listener,BACKLOG);

	t_Mensaje tipoSolicitud = recibir_header(socket_cliente);
	responder_solicitud(tipoSolicitud);

	//
	/*
	t_Mensaje header;
	recv(cliente,&header,sizeof header, 0);
	if(header == SOLICITUDPROCESAMIENTO){
		printf("Solicitud de procesamiento recibida");
	};
	*/
};
