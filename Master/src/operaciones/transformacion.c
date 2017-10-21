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


extern sem_t threadManager;
extern t_log* logger;

void* rutina_transformacion(void* args);
typedef struct{
	t_log*                       logger;
	payload_INFO_TRANSFORMACION* payload;
}DATA_TRANSFORMACION;

STATUS_MASTER transformacion (int socketYAMA, void* data){
	void*               payload;
	HEADER_T            header;
	pthread_t           tid;
	pthread_attr_t      attr;
	DATA_TRANSFORMACION data_transformacion;

	int cantidadDeOperaciones = 1;

	// Primer hilo de transformacion
		// Cargar datos
		data_transformacion.logger = logger;
		data_transformacion.payload = data;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&tid, &attr, rutina_transformacion, &data_transformacion);
	//TODO Destruir data

	// Recibir mas informaciones
	payload = receive(socketYAMA,&header);
	while(header == INFO_TRANSFORMACION){
		cantidadDeOperaciones ++;
		data_transformacion.payload = payload;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		pthread_create(&tid, &attr, rutina_transformacion, &data_transformacion);

		payload = receive(socketYAMA,&header);
	}

	// Esperar a que terminen todos los hilos para pasar a la siguiente etapa
	int i;
	for(i = 0; i < cantidadDeOperaciones ; i++ ){
		sem_wait(&threadManager);
	}

	return EXITO;
}

void* rutina_transformacion(void* args){
	HEADER_T header;

	DATA_TRANSFORMACION*         data    = args;
	t_log*                       logger  = data->logger;
	payload_INFO_TRANSFORMACION* payload = data->payload;

	int socketWorker = crear_conexion(payload->IP_Worker,payload->PUERTO_Worker);
	send_ORDEN_TRANSFORMACION(socketWorker,payload->bloque,payload->bytesocupados,payload->nombreArchivoTemporal);

	receive(socketWorker,&header);
	if(header == FIN_LISTA){
		log_info(logger, "Transformacion completada en %s:%d/BLOCK: %d",payload->IP_Worker,payload->PUERTO_Worker,payload->bloque);
	}else{
		log_error(logger, "Transformacion interrumpida en %s:%d/BLOCK: %d",payload->IP_Worker,payload->PUERTO_Worker,payload->bloque);
	}
	close(socketWorker);
	// TODO Destruir payload
	sem_post(&threadManager);
	pthread_exit(0);
}
