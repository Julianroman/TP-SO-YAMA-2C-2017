/*
 * reduccionGlobal.c
 *
 *  Created on: 21/10/2017
 *      Author: utnso
 */

#include <stdio.h>
#include <unistd.h>
#include <utilidades/protocol/senders.h>
#include <utilidades/protocol/receive.h>
#include <utilidades/protocol/destroy.h>
#include <utilidades/protocol/types.h>
#include <commons/collections/queue.h>
#include <utilidades/socket_utils.h>
#include <commons/log.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include "operaciones.h"
#include "../funcionesMaster.h"

extern t_log* logger;
extern char* rutaReductor;
extern double tiempoReduxGlobal;
extern int masterID;

STATUS_MASTER reduccionGlobal(int socketYAMA, void* data){
	log_trace(logger, "Reduccion global iniciada");


	// Iniciar timer
	time_t inicioEtapa,finEtapa;
	time (&inicioEtapa);
	HEADER_T            header;
	payload_INFO_REDUCCIONGLOBAL* payload   = data;
	payload_INFO_REDUCCIONGLOBAL* payloadEncargado;


	// Crear una cola para guardar las instrucciones a mandar
	t_queue * colaDeInformaciones = queue_create();


	log_trace(logger, "Recibiendo instrucciones...");
	// Separo al encargado
	if((payload -> encargado) == (payload -> ID_Nodo)){
		log_trace(logger, "Encargado seleccionado.");
		payloadEncargado  = payload;
	}else{
		queue_push(colaDeInformaciones,payload);
	}

	// Recibir mas instrucciones del yama
	payload = receive(socketYAMA,&header);
	while(header == INFO_REDUCCIONGLOBAL){
		if((payload -> encargado) == (payload -> ID_Nodo)){
			log_info(logger, "Encargado seleccionado.");
			payloadEncargado  = payload;
		}else{
			queue_push(colaDeInformaciones,payload);
		}
		payload = receive(socketYAMA,&header);
	}
	// Aseguro que termino la lista
	if(header != FIN_LISTA){
		log_error(logger,"Se esperaba un fin de lista");
		send_RESPUESTA_MASTER(socketYAMA,masterID,-1,-1,0);
		exit(1);
	}
	if (header == FIN_COMUNICACION){
		log_error(logger,"El administrador se ha desconectado");
		exit(1);
	}
	log_trace(logger, "Conectandose al encargado | Nodo%d  (%s:%d) ...",payloadEncargado -> ID_Nodo,payloadEncargado->IP_Worker,payloadEncargado->PUERTO_Worker);


	// Conectarse al encargado
	int socketWorker = crear_conexion(payloadEncargado->IP_Worker,payloadEncargado->PUERTO_Worker);
	if(socketWorker == -1){
		log_error(logger, "Redux global ERR | Nodo%d (%s:%d)",payloadEncargado -> ID_Nodo,payloadEncargado->IP_Worker,payloadEncargado->PUERTO_Worker);
		send_RESPUESTA_MASTER(socketYAMA,masterID,(payloadEncargado -> ID_Nodo),-1,0);
	}
	send_ORDEN_REDUCCIONGLOBAL(socketWorker,payloadEncargado->PUERTO_Worker,payloadEncargado->IP_Worker,payloadEncargado->nombreTemporal_ReduccionLocal,payloadEncargado->nombreTemporal_ReduccionGlobal,payloadEncargado->encargado);


	// Enviar nodos subordinados

	log_trace(logger, "Enviando instrucciones al encargado...");
	int i;
	payload_INFO_REDUCCIONGLOBAL*  payloadSubordinado;
	int cantidadNodos = queue_size(colaDeInformaciones);
	//log_trace(logger, cantidadNodos);
	for(i = 0; i < cantidadNodos;i++){
		payloadSubordinado = queue_pop(colaDeInformaciones);
		send_ORDEN_REDUCCIONGLOBAL(socketWorker,payloadSubordinado->PUERTO_Worker,payloadSubordinado->IP_Worker,payloadSubordinado->nombreTemporal_ReduccionLocal,payloadSubordinado->nombreTemporal_ReduccionGlobal,payloadSubordinado->encargado);

		// Destruir payload
		destroy_INFO_REDUCCIONGLOBAL(payloadSubordinado);
	}
	send_FIN_LISTA(socketWorker);

	// Envio de script
	int scriptFD = open(rutaReductor,O_RDONLY,0);
	int scriptSize = getScritptSize(rutaReductor);
	char * contenidoScript = leerScript(scriptSize, scriptFD);

	//Enviar contenido
	log_trace(logger,"Enviando script: %s (%d bytes)",rutaReductor,scriptSize);
	send_BLOQUE(socketWorker,scriptSize,contenidoScript,0);


	// Recibir respuesta y contactar al yama
	receive(socketWorker,&header);
	if(header == EXITO_OPERACION){
		log_info(logger, "Redux global OK | Nodo%d (%s:%d)",payloadEncargado -> ID_Nodo,payloadEncargado->IP_Worker,payloadEncargado->PUERTO_Worker);
		send_RESPUESTA_MASTER(socketYAMA,masterID,(payloadEncargado -> ID_Nodo),-1,1);
	} else {
		log_error(logger, "Redux global ERR | Nodo%d (%s:%d)",payloadEncargado -> ID_Nodo,payloadEncargado->IP_Worker,payloadEncargado->PUERTO_Worker);
		send_RESPUESTA_MASTER(socketYAMA,masterID,(payloadEncargado -> ID_Nodo),-1,0);
	}

	// Liberar recursos
	close(socketWorker);
	queue_destroy(colaDeInformaciones);


	// Parar timer
	time (&finEtapa);
	tiempoReduxGlobal += difftime(finEtapa,inicioEtapa);


	return EXITO;
};
