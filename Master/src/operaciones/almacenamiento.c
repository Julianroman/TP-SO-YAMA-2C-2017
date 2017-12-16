/*
 * almacenamiento.h
 *
 *  Created on: 21/10/2017
 *      Author: utnso
 */

#include <stdio.h>
#include <unistd.h>
#include <utilidades/protocol/senders.h>
#include <utilidades/protocol/types.h>
#include <utilidades/protocol/receive.h>
#include <utilidades/protocol/destroy.h>
#include <utilidades/socket_utils.h>
#include <commons/log.h>
#include <pthread.h>
#include <semaphore.h>
#include "operaciones.h"
#include "../funcionesMaster.h"

extern sem_t fin_job;
extern t_log* logger;
extern double tiempoAlmacenamiento;
extern int masterID;

STATUS_MASTER almacenamiento(int socketYAMA, void* data){
	time_t inicioEtapa,finEtapa;
	time (&inicioEtapa);
	log_info(logger, "Almacenamiento final iniciado");
	payload_INFO_ALMACENAMIENTO* payload = data;
	int socketWorker = crear_conexion(payload->IP_Worker,payload->PUERTO_Worker);
	if(socketWorker == -1){
		log_error(logger, "Almacenamiento de <%s> interrumpido por %s:%d",payload->nombreTemporal_ReduccionGlobal,payload->IP_Worker,payload->PUERTO_Worker);
		send_RESPUESTA_MASTER(socketYAMA,masterID,(payload -> ID_Nodo),-2,0);
	}

	puts("Ingrese la ruta donde desea guardar la transformacion:\n");
	char * rutaAlmacenamiento = inputGet();
	puts("Ingrese el nombre del archivo almacenado:\n");
	char * nombreAlmacenamiento = inputGet();

	send_ORDEN_ALMACENAMIENTO(socketWorker,rutaAlmacenamiento,nombreAlmacenamiento,payload->nombreTemporal_ReduccionGlobal);

	HEADER_T header;
	receive(socketWorker,&header);
	if(header == EXITO_OPERACION){
		log_info(logger, "Almacenamiento de <%s> completado por %s:%d",payload->nombreTemporal_ReduccionGlobal,payload->IP_Worker,payload->PUERTO_Worker);
		send_RESPUESTA_MASTER(socketYAMA,masterID,(payload -> ID_Nodo),-2,1);
	}else{
		log_error(logger, "Almacenamiento de <%s> interrumpido por %s:%d",payload->nombreTemporal_ReduccionGlobal,payload->IP_Worker,payload->PUERTO_Worker);
		send_RESPUESTA_MASTER(socketYAMA,masterID,(payload -> ID_Nodo),-2,0);
	}

	free(nombreAlmacenamiento);
	free(rutaAlmacenamiento);


	close(socketWorker);

	// Destruir payload
	destroy_INFO_ALMACENAMIENTO(payload);

	log_info(logger, "Almacenamiento finalizado");
	time (&finEtapa);
	tiempoAlmacenamiento += difftime(finEtapa,inicioEtapa);
	sem_post(&fin_job);
	return EXITO;
};
