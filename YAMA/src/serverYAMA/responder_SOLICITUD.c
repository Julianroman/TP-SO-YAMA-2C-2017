/*
 * responder_SOLICITUD.c
 */

#include <utilidades/protocol/types.h>
#include <commons/collections/dictionary.h>
#include <stdio.h>
#include "responses.h"

YAMA_STATUS responder_SOLICITUD(int socket, HEADER_T tipoDeMensaje,void* data){
	YAMA_STATUS status;
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
	printf("SOLICITUD_JOB recibida\n");
	printf("Archivo: %s, enviando informacion sobre workers\n",payload->nombreArchivo);

	//MANDAR A PLANIFICAR
	inciarPlanificacion();

	//DUMMIE WORKERS
	//Esto no lo hace el planificador?
	send_INFO_TRANSFORMACION(socket,9095,"127.0.0.1",38,10180,"/tmp/Master1-temp38");
	send_INFO_TRANSFORMACION(socket,9095,"127.0.0.1",39,10180,"/tmp/Master1-temp39");
	send_INFO_TRANSFORMACION(socket,9095,"127.0.0.1",44,10180,"/tmp/Master1-temp44");
	send_FIN_LISTA(socket);

	// IMPORTANTE! HAY UN LEAK DE MEMORIA
	// LA FUNCION DE RECOLECCION ESTA EN DESARROLLO
	//Sigue en desarrollo?
	return EXITO;
};

YAMA_STATUS res_SOLICITUD_INFO(int socket, void* data, HEADER_T tipoMensaje){

	switch(tipoMensaje){
	case INFO_TRANSFORMACION:
		payload_INFO_TRANSFORMACION* payload_transformacion = data;
		agregarAListaDeInstrucciones(payload_transformacion);
		break;
	case INFO_REDUCCIONLOCAL:
		payload_INFO_REDUCCIONLOCAL* payload_reduccionLocal = data;
		agregarAListaDeInstrucciones(payload_reduccionLocal);
		break;
	case INFO_REDUCCIONGLOBAL:
		payload_INFO_REDUCCIONGLOBAL* payload_reduccionGlobal = data;
		agregarAListaDeInstrucciones(payload_reduccionGlobal);
		break;
	case INFO_ALMACENAMIENTO:
		payload_INFO_ALMACENAMIENTO* payload_almacenamiento = data;
		agregarAListaDeInstrucciones(payload_almacenamiento);
		break;
	default:
		break;
	}

	return EXITO;

}
