/*
 * reduccionLocal.c
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

void* rutina_reduccionLocal(void* args);
typedef struct{
	t_log*                       logger;
	payload_INFO_REDUCCIONLOCAL* payload;
}DATA_REDUCCIONLOCAL;

STATUS_MASTER reduccionLocal (int socketYAMA, void* data){
	void*               payload;
	HEADER_T            header;
	pthread_t           tid;
	pthread_attr_t      attr;
	DATA_REDUCCIONLOCAL data_reduccionLocal;

	int cantidadDeOperaciones = 1;

	// Primer hilo de transformacion
		// Cargar datos
		data_reduccionLocal.logger = logger;
		data_reduccionLocal.payload = data;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&tid, &attr, rutina_reduccionLocal, &data_reduccionLocal);
	//TODO Destruir data

	// Recibir mas informaciones
	payload = receive(socketYAMA,&header);
	while(header == INFO_REDUCCIONLOCAL){
		cantidadDeOperaciones ++;
		data_reduccionLocal.payload = payload;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		pthread_create(&tid, &attr, rutina_reduccionLocal, &data_reduccionLocal);

		payload = receive(socketYAMA,&header);
	}

	// Esperar a que terminen todos los hilos para pasar a la siguiente etapa
	int i;
	for(i = 0; i < cantidadDeOperaciones; i++ ){
		sem_wait(&threadManager);
	}

	return EXITO;
}

void* rutina_reduccionLocal(void* args){
	HEADER_T header;

	DATA_REDUCCIONLOCAL*         data    = args;
	t_log*                       logger  = data->logger;
	payload_INFO_REDUCCIONLOCAL* payload = data->payload;

	int socketWorker = crear_conexion(payload->IP_Worker,payload->PUERTO_Worker);
	send_ORDEN_REDUCCIONLOCAL(socketWorker,payload->nombreTemporal_Transformacion,payload->nombreTemporal_ReduccionLocal);

	receive(socketWorker,&header);
	if(header == FIN_LISTA){
		log_info(logger, "Reduccion local completada en %s:%d // %s ---> %s",payload->IP_Worker,payload->PUERTO_Worker,payload->nombreTemporal_Transformacion,payload->nombreTemporal_ReduccionLocal);
	}else{
		log_error(logger, "Reduccion local interrumpida en %s:%d // %s -/-> %s",payload->IP_Worker,payload->PUERTO_Worker,payload->nombreTemporal_Transformacion,payload->nombreTemporal_ReduccionLocal);
	}
	close(socketWorker);
	// TODO Destruir payload
	sem_post(&threadManager);
	pthread_exit(0);
}


