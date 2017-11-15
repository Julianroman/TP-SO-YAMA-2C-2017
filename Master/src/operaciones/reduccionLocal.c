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

extern t_log* logger;
extern sem_t reductionThreads;
extern char* scriptReductor;

void* rutina_reduccionLocal(void* args);


STATUS_MASTER reduccionLocal (int socketYAMA, payload_INFO_REDUCCIONLOCAL* data){
	pthread_t           tid;
	pthread_attr_t      attr;


	log_trace(logger, "Reduccion local iniciada");

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&tid, &attr, rutina_reduccionLocal, data);

	return EXITO;
}

void* rutina_reduccionLocal(void* args){
	HEADER_T header;
	payload_INFO_REDUCCIONLOCAL* payload = args;

	// Enviar orden
	int socketWorker = crear_conexion(payload->IP_Worker,payload->PUERTO_Worker);
	send_ORDEN_REDUCCIONLOCAL(socketWorker,payload->nombreTemporal_Transformacion,payload->nombreTemporal_ReduccionLocal);

	send_SCRIPT(socketWorker,scriptReductor);

	// Recibir resultado
	receive(socketWorker,&header);
	if(header == EXITO_OPERACION){
		log_info(logger, "Redux local OK %s:%d // %s ---> %s",payload->IP_Worker,payload->PUERTO_Worker,payload->nombreTemporal_Transformacion,payload->nombreTemporal_ReduccionLocal);
	}
	else if(header == FIN_COMUNICACION || header == FRACASO_OPERACION){
		log_error(logger, "Redux local ERR %s:%d // %s -/-> %s",payload->IP_Worker,payload->PUERTO_Worker,payload->nombreTemporal_Transformacion,payload->nombreTemporal_ReduccionLocal);
	}
	else{
		log_warning(logger,"No se reconoce la respuesta del worker");
	}

	close(socketWorker);
	// TODO Destruir payload
	sem_post(&reductionThreads);
	pthread_exit(0);
}


