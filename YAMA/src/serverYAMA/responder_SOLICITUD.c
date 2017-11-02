/*
 * responder_SOLICITUD.c
 */

#include <utilidades/protocol/types.h>
#include <commons/collections/dictionary.h>
#include <stdio.h>
#include "responses.h"
#include "../YAMA.h"

YAMA_STATUS responder_SOLICITUD(int socket,void* data,HEADER_T tipoDeMensaje){
	YAMA_STATUS status;
	if(tipoDeMensaje == SOLICITUD_JOB){
		status = res_SOLICITUD_JOB(socket, data);
	}
	else if(tipoDeMensaje == RESPUESTA_MASTER){
		status = res_RECIBIR_INFO(socket, data);
	}
	return status;
}

YAMA_STATUS res_SOLICITUD_JOB(int socket,void*  data){
	payload_SOLICITUD_JOB* payload = data;
	log_trace(logYAMA,"SOLICITUD_JOB recibida\n");

	//MANDAR A PLANIFICAR
	inciarPlanificacion(payload->nombreArchivo);

	return EXITO;
}

YAMA_STATUS res_RECIBIR_INFO(int socket, void* data){
	payload_RESPUESTA_MASTER* infoMaster = data;
	agregarAListaInfoMaster(&infoMaster);
	log_trace(logYAMA, "Informacion de master recibida");
	return EXITO;
}
