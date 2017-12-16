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
#include <utilidades/protocol/destroy.h>
#include <utilidades/socket_utils.h>
#include <commons/log.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include "operaciones.h"
#include "../funcionesMaster.h"

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
extern sem_t balancer;
extern char * rutaTransformador;

int YAMAsocket;

void* rutina_transformacion(void* args);

STATUS_MASTER transformacion (int socketYAMA, payload_INFO_TRANSFORMACION* data){
	sem_wait(&balancer);
	YAMAsocket = socketYAMA;
	pthread_t           tid;
	pthread_attr_t      attr;

	log_trace(logger, "Transformacion iniciada | Nodo%d (%s:%d) BLOQUE:%d",data->ID_Nodo,data->IP_Worker,data->PUERTO_Worker,data->bloque);
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
	if ( socketWorker == -1){
		fallosTransformacion ++;
		log_error(logger, "Transformacion ERR (No route to host)| Nodo%d (%s:%d) BLOQUE:%d",payload->ID_Nodo,payload->IP_Worker,payload->PUERTO_Worker,payload->bloque);
		send_RESPUESTA_MASTER(YAMAsocket,masterID,idNodo,payload->bloque,0);

	}
	send_ORDEN_TRANSFORMACION(socketWorker,payload->bloque,payload->bytesocupados,payload->nombreArchivoTemporal);

	// Envio de script
	int scriptFD = open(rutaTransformador,O_RDONLY,0);
	int scriptSize = getScritptSize(rutaTransformador);
	char * contenidoScript = leerScript(scriptSize, scriptFD);

	//Enviar contenido
	send_BLOQUE(socketWorker,scriptSize,contenidoScript,0);

	// Recibir resultado
	receive(socketWorker,&header);
	if(header == EXITO_OPERACION){
		log_info(logger, "Transformacion OK | Nodo%d (%s:%d) BLOQUE:%d",payload->ID_Nodo,payload->IP_Worker,payload->PUERTO_Worker,payload->bloque);
		send_RESPUESTA_MASTER(YAMAsocket,masterID,idNodo,payload->bloque,1);
	}else{
		fallosTransformacion ++;
		log_error(logger, "Transformacion ERR | Nodo%d (%s:%d) BLOQUE:%d",payload->ID_Nodo,payload->IP_Worker,payload->PUERTO_Worker,payload->bloque);
		send_RESPUESTA_MASTER(YAMAsocket,masterID,idNodo,payload->bloque,0);
	}
	close(socketWorker);

	// Destruir payload
	destroy_INFO_TRANSFORMACION(payload);

	// Parar timer y actualizar
	time (&finEtapa);
	tiempoTransformacion += difftime(finEtapa,inicioEtapa);

	// Verificacion para estadisticas
	transformacionesEnProceso --;
	paralelasEnProceso--;

	sem_post(&balancer);

	pthread_exit(0);
}
