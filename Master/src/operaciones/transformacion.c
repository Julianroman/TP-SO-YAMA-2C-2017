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


extern double tiempoTransformacion;
extern t_log* logger;
extern char* scriptTransformador;
extern int transformacionesRealizadas;
extern int fallosTransformacion;
extern int transformacionesEnProceso;
extern int paralelasEnProceso;
extern int maxTransformacionesEnProceso;
extern int maxParalelasEnProceso;
extern int masterID;
int YAMAsocket;

void* rutina_transformacion(void* args);

STATUS_MASTER transformacion (int socketYAMA, payload_INFO_TRANSFORMACION* data){
	YAMAsocket = socketYAMA;
	pthread_t           tid;
	pthread_attr_t      attr;

	log_trace(logger, "Transformacion iniciada");
	transformacionesRealizadas ++;

	// Verificacion para estadisticas
	transformacionesEnProceso ++;
	if(transformacionesEnProceso > maxTransformacionesEnProceso) maxTransformacionesEnProceso = transformacionesEnProceso;
	paralelasEnProceso++;
	if(paralelasEnProceso > maxParalelasEnProceso) maxParalelasEnProceso = paralelasEnProceso;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&tid, &attr, rutina_transformacion, data);

	return EXITO;
}

void* rutina_transformacion(void* args){
	time_t inicioEtapa,finEtapa;
	HEADER_T header;
	payload_INFO_TRANSFORMACION* payload = args;
	int idNodo = payload -> ID_Nodo;

	// Iniciar timer
	time (&inicioEtapa);

	// Enviar orden
	int socketWorker = crear_conexion(payload->IP_Worker,payload->PUERTO_Worker);
	send_ORDEN_TRANSFORMACION(socketWorker,payload->bloque,payload->bytesocupados,payload->nombreArchivoTemporal);

	// Enviar script
	send_SCRIPT(socketWorker,scriptTransformador);

	// Recibir resultado
	receive(socketWorker,&header);
	if(header == EXITO_OPERACION){
		log_info(logger, "Transformacion OK %s:%d // BLOCK: %d",payload->IP_Worker,payload->PUERTO_Worker,payload->bloque);
		send_RESPUESTA_MASTER(YAMAsocket,masterID,idNodo,payload->bloque,1);
	}
	else if(header == FIN_COMUNICACION || header == FRACASO_OPERACION){
		fallosTransformacion ++;
		log_error(logger, "Transformacion ERR %s:%d // BLOCK: %d",payload->IP_Worker,payload->PUERTO_Worker,payload->bloque);
		send_RESPUESTA_MASTER(YAMAsocket,masterID,idNodo,payload->bloque,0);
	}
	else{
		fallosTransformacion ++;
		log_warning(logger,"No se reconoce la respuesta del worker");
		log_error(logger, "Transformacion ERR %s:%d // BLOCK: %d",payload->IP_Worker,payload->PUERTO_Worker,payload->bloque);
		send_RESPUESTA_MASTER(YAMAsocket,masterID,idNodo,payload->bloque,0);
		exit(1);
	}
	close(socketWorker);
	// TODO Destruir payload

	// Parar timer y actualizar
	time (&finEtapa);
	tiempoTransformacion += difftime(finEtapa,inicioEtapa);

	// Verificacion para estadisticas
	transformacionesEnProceso --;
	paralelasEnProceso--;

	pthread_exit(0);
}
