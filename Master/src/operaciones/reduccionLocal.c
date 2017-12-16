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
#include <utilidades/protocol/destroy.h>
#include <utilidades/socket_utils.h>
#include <commons/log.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include "operaciones.h"
#include "../funcionesMaster.h"

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
extern sem_t recepcionSem;
extern char* rutaReductor;

int YAMAsocket;

void* rutina_reduccionLocal(void* args);


STATUS_MASTER reduccionLocal (int socketYAMA, payload_INFO_REDUCCIONLOCAL* data){
	YAMAsocket = socketYAMA;
	pthread_t           tid;
	pthread_attr_t      attr;

	log_trace(logger, "Reduccion local iniciada | Nodo%d (%s:%d) ",data -> ID_Nodo,data->IP_Worker,data->PUERTO_Worker);
	reduxLocalesRealizadas ++;

	// Verificacion para estadisticas
	reduxLocalesEnProceso ++;
	if(reduxLocalesEnProceso > maxReduxLocalesEnProceso) maxReduxLocalesEnProceso = reduxLocalesEnProceso;
	paralelasEnProceso++;
	if(paralelasEnProceso > maxParalelasEnProceso) maxParalelasEnProceso = paralelasEnProceso;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&tid, &attr, rutina_reduccionLocal, data);

	// Esperar que el hilo reciba las informaciones necesarias
	sem_wait(&recepcionSem);

	return EXITO;
}

void* rutina_reduccionLocal(void* args){
	time_t inicioEtapa,finEtapa;
	HEADER_T header;
	payload_INFO_REDUCCIONLOCAL* payload = args;
	int idNodo = payload -> ID_Nodo;


	// Iniciar timer
	time (&inicioEtapa);


	// Guardo informacion reelevante para loggear luego
	char* nombreReduccionLocal= malloc(strlen(payload -> nombreTemporal_ReduccionLocal)+1);
	char* ipWorker = malloc(strlen(payload -> IP_Worker)+1);
	strcpy(nombreReduccionLocal,payload -> nombreTemporal_ReduccionLocal);
	strcpy(ipWorker,payload -> IP_Worker);
	int puetoWorker = payload -> PUERTO_Worker;


	// Enviar orden inicial
	int socketWorker = crear_conexion(payload->IP_Worker,payload->PUERTO_Worker);
	if(socketWorker == -1){
		fallosReduxLocal ++;
		log_error(logger, "Redux local ERR | Nodo %d (%s:%d)",idNodo,ipWorker,puetoWorker);
		send_RESPUESTA_MASTER(YAMAsocket,masterID,idNodo,-1,0);
	}
	send_ORDEN_REDUCCIONLOCAL(socketWorker,payload->nombreTemporal_Transformacion,payload->nombreTemporal_ReduccionLocal);
	// Destruir lo que no uso
	destroy_INFO_REDUCCIONLOCAL(payload);


	// Recibir mas reducciones del mismo nodo
	payload = receive(YAMAsocket,&header);
	while(header == INFO_REDUCCIONLOCAL){
		// Enviar
		send_ORDEN_REDUCCIONLOCAL(socketWorker,payload->nombreTemporal_Transformacion,payload->nombreTemporal_ReduccionLocal);
		// Destruir
		destroy_INFO_REDUCCIONLOCAL(payload);
		// Repeat
		payload = receive(YAMAsocket,&header);
	}

	// Instruccion inesperada
	if(header != FIN_LISTA){log_error(logger, "Fin de lista esperado, Cerrando Master ..."); exit(1);}


	// Enviar fin de lista al Worker
	send_FIN_LISTA(socketWorker);


	// Indicar fin de recepcion
	sem_post(&recepcionSem);


	// Envio de script
	int scriptFD = open(rutaReductor,O_RDONLY,0);
	int scriptSize = getScritptSize(rutaReductor);
	char * contenidoScript = leerScript(scriptSize, scriptFD);

	//Enviar contenido
	log_info(logger,"Enviando script: %s (%d bytes)",rutaReductor,scriptSize);
	send_BLOQUE(socketWorker,scriptSize,contenidoScript,0);


	// Recibir resultado
	receive(socketWorker,&header);
	if(header == EXITO_OPERACION){
		log_info(logger, "Redux local OK | Nodo %d (%s:%d)",idNodo,ipWorker,puetoWorker);
		send_RESPUESTA_MASTER(YAMAsocket,masterID,idNodo,-1,1);
	}else{
		fallosReduxLocal ++;
		log_error(logger, "Redux local ERR | Nodo %d (%s:%d)",idNodo,ipWorker,puetoWorker);
		send_RESPUESTA_MASTER(YAMAsocket,masterID,idNodo,-1,0);
	}

	// Libero recursos
	close(socketWorker);
	free(nombreReduccionLocal);
	free(ipWorker);


	// Verificacion para estadisticas
	reduxLocalesEnProceso --;
	paralelasEnProceso--;


	// Parar timer y actualizar
	time (&finEtapa);
	tiempoReduxLocal += difftime(finEtapa,inicioEtapa);

	pthread_exit(0);
}


