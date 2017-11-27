/*
 * reduccionGlobal.c
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
#include <commons/collections/queue.h>
#include <pthread.h>
#include <semaphore.h>
#include "operaciones.h"

extern t_log* logger;
extern char* scriptReductor;
extern double tiempoReduxGlobal;
extern int masterID;

STATUS_MASTER reduccionGlobal(int socketYAMA, void* data){
	time_t inicioEtapa,finEtapa;
	time (&inicioEtapa);
	log_trace(logger, "Reduccion global iniciada");
	HEADER_T            header;
	payload_INFO_REDUCCIONGLOBAL* payload   = data;
	payload_INFO_REDUCCIONGLOBAL* payloadEncargado;

	t_queue * colaDeInformaciones = queue_create();


	if(payload -> encargado == 0){
		queue_push(colaDeInformaciones,payload);
	}else if(payload -> encargado == 1){
		payloadEncargado  = payload;
	}
	// TODO destruir payload
	// Recibir mas informaciones
	payload = receive(socketYAMA,&header);
	while(header == INFO_REDUCCIONGLOBAL){
		if(payload -> encargado == 0){
			queue_push(colaDeInformaciones,payload);
		}else if(payload -> encargado == 1){
			payloadEncargado  = payload;
		}
		payload = receive(socketYAMA,&header);
	}

	// Conectarse al encargado
	int socketWorker = crear_conexion(payloadEncargado->IP_Worker,payloadEncargado->PUERTO_Worker);
	send_ORDEN_REDUCCIONGLOBAL(socketWorker,payloadEncargado->PUERTO_Worker,payloadEncargado->IP_Worker,payloadEncargado->nombreTemporal_ReduccionLocal,payloadEncargado->nombreTemporal_ReduccionGlobal,payloadEncargado->encargado);

	// Enviar nodos subordinados
	int i;
	payload_INFO_REDUCCIONGLOBAL*  payloadSubordinado;
	for(i = 0; i < queue_size(colaDeInformaciones);i++){
		payloadSubordinado = queue_pop(colaDeInformaciones);
		send_ORDEN_REDUCCIONGLOBAL(socketWorker,payloadSubordinado->PUERTO_Worker,payloadSubordinado->IP_Worker,payloadSubordinado->nombreTemporal_ReduccionLocal,payloadSubordinado->nombreTemporal_ReduccionGlobal,payloadSubordinado->encargado);
		// TODO destruir payload
	}
	send_FIN_LISTA(socketWorker);

	send_SCRIPT(socketWorker,scriptReductor);

	receive(socketWorker,&header);
	if(header == EXITO_OPERACION){
		log_info(logger, "Reduccion global completada en %s:%d",payloadEncargado->IP_Worker,payloadEncargado->PUERTO_Worker);
		send_RESPUESTA_MASTER(socketYAMA,masterID,-1,-1,0);
	}
	else if(header == FIN_COMUNICACION || header == FRACASO_OPERACION){
		// TODO Corregir la info del logger
		//log_error(logger, "Reduccion global interrumpida en %s:%d",payloadEncargado->IP_Worker,payloadEncargado->PUERTO_Worker);
		log_error(logger, "Redux global ERR ");
		send_RESPUESTA_MASTER(socketYAMA,masterID,-1,-1,1);
	}
	else{
		log_warning(logger,"No se reconoce la respuesta del worker");
	}

	close(socketWorker);
	queue_destroy(colaDeInformaciones);

	log_trace(logger, "Reduccion global finalizada");
	time (&finEtapa);
	tiempoReduxGlobal += difftime(finEtapa,inicioEtapa);
	return EXITO;
};
