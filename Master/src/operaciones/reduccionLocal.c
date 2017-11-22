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
extern double tiempoReduxLocal;
extern char* scriptReductor;
extern int reduxLocalesRealizadas;
extern int fallosReduxLocal;
extern int reduxLocalesEnProceso;
extern int paralelasEnProceso;
extern int maxReduxLocalesEnProceso;
extern int maxParalelasEnProceso;
extern int masterID;

int YAMAsocket;

void* rutina_reduccionLocal(void* args);


STATUS_MASTER reduccionLocal (int socketYAMA, payload_INFO_REDUCCIONLOCAL* data){
	YAMAsocket = socketYAMA;
	pthread_t           tid;
	pthread_attr_t      attr;

	log_trace(logger, "Reduccion local iniciada");
	reduxLocalesRealizadas ++;

	// Verificacion para estadisticas
	reduxLocalesEnProceso ++;
	if(reduxLocalesEnProceso > maxReduxLocalesEnProceso) maxReduxLocalesEnProceso = reduxLocalesEnProceso;
	paralelasEnProceso++;
	if(paralelasEnProceso > maxParalelasEnProceso) maxParalelasEnProceso = paralelasEnProceso;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&tid, &attr, rutina_reduccionLocal, data);

	return EXITO;
}

void* rutina_reduccionLocal(void* args){
	time_t inicioEtapa,finEtapa;
	HEADER_T header;
	payload_INFO_REDUCCIONLOCAL* payload = args;

	// Iniciar timer
	time (&inicioEtapa);

	// Enviar orden
	int socketWorker = crear_conexion(payload->IP_Worker,payload->PUERTO_Worker);
	send_ORDEN_REDUCCIONLOCAL(socketWorker,payload->nombreTemporal_Transformacion,payload->nombreTemporal_ReduccionLocal);

	// Enviar script
	send_SCRIPT(socketWorker,scriptReductor);

	// Recibir resultado
	receive(socketWorker,&header);
	if(header == EXITO_OPERACION){
		log_info(logger, "Redux local OK %s:%d // %s ---> %s",payload->IP_Worker,payload->PUERTO_Worker,payload->nombreTemporal_Transformacion,payload->nombreTemporal_ReduccionLocal);
		send_RESPUESTA_MASTER(YAMAsocket,masterID,-1,-1,0);
	}
	else if(header == FIN_COMUNICACION || header == FRACASO_OPERACION){
		fallosReduxLocal ++;
		log_error(logger, "Redux local ERR %s:%d // %s -/-> %s",payload->IP_Worker,payload->PUERTO_Worker,payload->nombreTemporal_Transformacion,payload->nombreTemporal_ReduccionLocal);
		send_RESPUESTA_MASTER(YAMAsocket,masterID,-1,-1,1);
	}
	else{
		log_warning(logger,"No se reconoce la respuesta del worker");
	}

	close(socketWorker);
	// TODO Destruir payload

	// Parar timer y actualizar
	time (&finEtapa);
	tiempoReduxLocal += difftime(finEtapa,inicioEtapa);

	// Verificacion para estadisticas
	reduxLocalesEnProceso --;
	paralelasEnProceso--;

	pthread_exit(0);
}


