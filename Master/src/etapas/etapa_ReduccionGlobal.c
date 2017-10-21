/*
 * etapa_transformacion.c
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
#include "etapas.h"

extern t_log* logger;


MASTER_STATUS etapa_reduccionGlobal (int socketYAMA){
	log_trace(logger, "Iniciando etapa de reduccion global...");

	payload_INFO_REDUCCIONGLOBAL* payload;
	payload_INFO_REDUCCIONGLOBAL* payloadEncargado;
	t_queue * colaDeInformaciones = queue_create();
	HEADER_T header;
	void* data;

	send_SOLICITUD_REDUCCIONGLOBAL(socketYAMA);
	data = receive(socketYAMA,&header);

	if (header == FIN_COMUNICACION){ /*Si header es FIN_COMUNICACION es porque se cerro la conexion*/ }

	// Recibir todas las instrucciones
	// Hasta que termine la lista
	while(header != FIN_LISTA){
		if (header == INFO_REDUCCIONGLOBAL){
			payload = data; // Casteo impicito
			if(payload->encargado ==  1){
				payloadEncargado  = payload;
			}
			if(payload->encargado ==  0){
				queue_push(colaDeInformaciones,payload);
			}
		}
		data = receive(socketYAMA,&header);
		if (header == FIN_COMUNICACION){ /*Si header es FIN_COMUNICACION es porque se cerro la conexion */}
	}

	// Conectarse al encargado
	int socketWorker = crear_conexion(payloadEncargado->IP_Worker,payloadEncargado->PUERTO_Worker);
	send_ORDEN_REDUCCIONGLOBAL(socketWorker,payload->PUERTO_Worker,payload->IP_Worker,payload->nombreTemporal_ReduccionLocal,payload->nombreTemporal_ReduccionGlobal,payload->encargado);

	int itr = 0;
	payload_INFO_REDUCCIONGLOBAL*  payloadSubordinado;
	printf("subordinados: %d\n",queue_size(colaDeInformaciones));
	for(itr;itr < queue_size(colaDeInformaciones);itr++){
		printf("%d",itr);
		payloadSubordinado = queue_pop(colaDeInformaciones);
		send_ORDEN_REDUCCIONGLOBAL(socketWorker,payloadSubordinado->PUERTO_Worker,payloadSubordinado->IP_Worker,payloadSubordinado->nombreTemporal_ReduccionLocal,payloadSubordinado->nombreTemporal_ReduccionGlobal,payloadSubordinado->encargado);
		// IMPORTANTE! HAY UN LEAK DE MEMORIA
		// LA FUNCION DE RECOLECCION ESTA EN DESARROLLO
	}
	send_FIN_LISTA(socketWorker);

	receive(socketWorker,&header);
	if(header == FIN_LISTA){
		log_info(logger, "Reduccion global completada en %s:%d",payloadEncargado->IP_Worker,payloadEncargado->PUERTO_Worker);
	}else{
		log_error(logger, "Reduccion global interrumpida en %s:%d",payloadEncargado->IP_Worker,payloadEncargado->PUERTO_Worker);
	}
	close(socketWorker);
	queue_destroy(colaDeInformaciones);

	return EXITO;
};

