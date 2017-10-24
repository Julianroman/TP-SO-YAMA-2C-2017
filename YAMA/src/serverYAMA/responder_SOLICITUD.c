/*
 * responder_SOLICITUD.c
 */

#include <utilidades/protocol/types.h>
#include <commons/collections/dictionary.h>
#include <stdio.h>
#include "responses.h"

YAMA_STATUS responder_SOLICITUD(int socket, HEADER_T tipoDeMensaje,void* data){
	YAMA_STATUS status = malloc(sizeof(YAMA_STATUS));
	if(tipoDeMensaje == SOLICITUD_JOB){
		status = res_SOLICITUD_JOB(socket, data);
		break;
	}
	else if(tipoDeMensaje == INFO_TRANSFORMACION || tipoDeMensaje == INFO_REDUCCIONLOCAL || tipoDeMensaje == INFO_REDUCCIONGLOBAL || tipoDeMensaje == INFO_ALMACENAMIENTO){
		status = res_SOLICITUD_INFO(socket, data, tipoDeMensaje);
	}
	return status;
}

YAMA_STATUS res_SOLICITUD_JOB(int socket,void*  data){
	payload_SOLICITUD_JOB* payload = data;
	log_trace(logYAMA,"SOLICITUD_JOB recibida\n");

	//MANDAR A PLANIFICAR
	inciarPlanificacion(payload->nombreArchivo);

	return EXITO;
};

YAMA_STATUS res_SOLICITUD_INFO(int socket, void* data, HEADER_T tipoMensaje){

	switch(tipoMensaje){
	case INFO_TRANSFORMACION:
		payload_INFO_TRANSFORMACION* payload_transformacion = data;
		respuestaInfoMaster* infoMasterTransformacion = malloc(sizeof(respuestaInfoMaster));
		//Agregar atributos del struct y mandarlo
		agregarAListaInfoMaster(infoMasterTransformacion);
		break;
	case INFO_REDUCCIONLOCAL:
		payload_INFO_REDUCCIONLOCAL* payload_reduccionLocal = data;
		respuestaInfoMaster* infoMasterReduccionLocal = malloc(sizeof(respuestaInfoMaster));
		//Agregar atributos del struct y mandarlo
		agregarAListaInfoMaster(infoMasterReduccionLocal);
		break;
	case INFO_REDUCCIONGLOBAL:
		payload_INFO_REDUCCIONGLOBAL* payload_reduccionGlobal = data;
		respuestaInfoMaster* infoMasterReduccionGlobal = malloc(sizeof(respuestaInfoMaster));
		//Agregar atributos del struct y mandarlo
		agregarAListaInfoMaster(infoMasterReduccionGlobal);
		break;
	case INFO_ALMACENAMIENTO:
		payload_INFO_ALMACENAMIENTO* payload_almacenamiento = data;
		respuestaInfoMaster* infoMasterAlmacenamiento = malloc(sizeof(respuestaInfoMaster));
		//Agregar atributos del struct y mandarlo
		agregarAListaInfoMaster(infoMasterAlmacenamiento);
		break;
	default:
		break;
	}

	return EXITO;

}
