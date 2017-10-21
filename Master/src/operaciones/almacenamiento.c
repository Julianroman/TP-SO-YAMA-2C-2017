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
#include <utilidades/socket_utils.h>
#include <commons/log.h>
#include <pthread.h>
#include <semaphore.h>
#include "operaciones.h"

extern t_log* logger;

STATUS_MASTER almacenamiento(int socketYAMA, void* data){
	log_trace(logger, "Almacenamiento final iniciado");
	payload_INFO_ALMACENAMIENTO* payload = data;
	int socketWorker = crear_conexion(payload->IP_Worker,payload->PUERTO_Worker);
	send_ORDEN_ALMACENAMIENTO(socketWorker,payload->nombreTemporal_ReduccionGlobal);

	HEADER_T header;
	receive(socketWorker,&header);
	if(header == FIN_LISTA){
		log_info(logger, "Almacenamiento de <%s> completado por %s:%d",payload->nombreTemporal_ReduccionGlobal,payload->IP_Worker,payload->PUERTO_Worker);
	}else{
		log_error(logger, "Almacenamiento de <%s> interrumpido por %s:%d",payload->nombreTemporal_ReduccionGlobal,payload->IP_Worker,payload->PUERTO_Worker);
	}
	close(socketWorker);
	// TODO destruir payload
	log_trace(logger, "Almacenamiento finalizado");
	return EXITO;
};
