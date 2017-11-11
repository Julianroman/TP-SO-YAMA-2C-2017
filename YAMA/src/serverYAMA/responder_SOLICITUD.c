/*
 * responder_SOLICITUD.cinciarPlanificacion
 */

#include <utilidades/protocol/types.h>
#include <commons/collections/dictionary.h>
#include <stdio.h>
#include "responses.h"
#include "../YAMA.h"
#include "../Planificador.h"

void responder_SOLICITUD(int socket,void* data,HEADER_T tipoDeMensaje){
	YAMA_STATUS status;
	if(tipoDeMensaje == SOLICITUD_JOB){
		status = res_SOLICITUD_JOB(socket, data);
	}
	else if(tipoDeMensaje == RESPUESTA_MASTER){
		status = res_RECIBIR_INFO(socket, data);
}

void res_SOLICITUD_JOB(int socket,void*  data){
	payload_SOLICITUD_JOB* payload = data;
	log_trace(logYAMA,"SOLICITUD_JOB recibida. Empezando planificacion\n");

	//MANDAR A PLANIFICAR
	iniciarPlanificacion(payload->nombreArchivo);
}

void res_RECIBIR_INFO(int socket, void* data){
	payload_RESPUESTA_MASTER* infoMaster = data;
	agregarAListaInfoMaster(infoMaster);
	log_trace(logYAMA, "Informacion de master recibida. Actualizando estados");
}
