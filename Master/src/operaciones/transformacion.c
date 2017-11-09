/*
 * transformacion.c
 *
 *  Created on: 21/10/2017
 *      Author: utnso
 */

#include <stdio.h>
#include <unistd.h>
#include <utilidades/protocol/senders.h>
#include <utilidades/protocol/types.h>
#include <utilidades/protocol/receive.h>
#include <utilidades/socket_utils.h>
#include <commons/log.h>
#include <pthread.h>
#include <semaphore.h>
#include "operaciones.h"


extern int transformacionesRestantes;
extern int transformador_fd;
extern t_log* logger;
extern pthread_mutex_t transformationManager;

void* rutina_transformacion(void* args);

STATUS_MASTER transformacion (int socketYAMA, payload_INFO_TRANSFORMACION* data){


	pthread_t           tid;
	pthread_attr_t      attr;

	log_trace(logger, "Transformacion iniciada");

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&tid, &attr, rutina_transformacion, data);

	return EXITO;
}

void* rutina_transformacion(void* args){


	HEADER_T header;
	payload_INFO_TRANSFORMACION* payload = args;

	// Enviar orden
	int socketWorker = crear_conexion(payload->IP_Worker,payload->PUERTO_Worker);
	send_ORDEN_TRANSFORMACION(socketWorker,payload->bloque,payload->bytesocupados,payload->nombreArchivoTemporal);

	send_ARCHIVO(socketWorker,transformador_fd);


	// Recibir resultado
	receive(socketWorker,&header);
	if(header == EXITO_OPERACION){
		log_info(logger, "Transformacion OK %s:%d // BLOCK: %d",payload->IP_Worker,payload->PUERTO_Worker,payload->bloque);
	}
	else if(header == FIN_COMUNICACION || header == FRACASO_OPERACION){
		log_error(logger, "Transformacion ERR %s:%d // BLOCK: %d",payload->IP_Worker,payload->PUERTO_Worker,payload->bloque);
	}
	else{
		log_warning(logger,"No se reconoce la respuesta del worker");
	}
	close(socketWorker);
	// TODO Destruir payload
	pthread_mutex_lock(&transformationManager);
		transformacionesRestantes--;
	pthread_mutex_unlock(&transformationManager);
	pthread_exit(0);
}
