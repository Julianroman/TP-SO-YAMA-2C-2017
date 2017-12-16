
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "types.h"
#include "packers.h"
#include "enviar_paquete.h"
#include "destroy.h"

void destroy(HEADER_T header,void* payload){
	switch(header) {
		case SOLICITUD_JOB:
			destroy_SOLICITUD_JOB((payload_SOLICITUD_JOB*) payload);
		break;
		case ORDEN_TRANSFORMACION:
			destroy_ORDEN_TRANSFORMACION((payload_ORDEN_TRANSFORMACION*) payload);
		break;
		case ORDEN_REDUCCIONLOCAL:
			destroy_ORDEN_REDUCCIONLOCAL((payload_ORDEN_REDUCCIONLOCAL*) payload);
		break;
		case ORDEN_REDUCCIONGLOBAL:
			destroy_ORDEN_REDUCCIONGLOBAL((payload_ORDEN_REDUCCIONGLOBAL*) payload);
		break;
		case ORDEN_ALMACENAMIENTO:
			destroy_ORDEN_ALMACENAMIENTO((payload_ORDEN_ALMACENAMIENTO*) payload);
		break;
		case INFO_TRANSFORMACION:
			destroy_INFO_TRANSFORMACION((payload_INFO_TRANSFORMACION*) payload);
		break;
		case INFO_REDUCCIONLOCAL:
			destroy_INFO_REDUCCIONLOCAL((payload_INFO_REDUCCIONLOCAL*) payload);
		break;
		case INFO_REDUCCIONGLOBAL:
			destroy_INFO_REDUCCIONGLOBAL((payload_INFO_REDUCCIONGLOBAL*) payload);
		break;
		case INFO_ALMACENAMIENTO:
			destroy_INFO_ALMACENAMIENTO((payload_INFO_ALMACENAMIENTO*) payload);
		break;
		case PETICION_NODO:
			destroy_PETICION_NODO((payload_PETICION_NODO*) payload);
		break;
		case NODO:
			destroy_NODO((payload_NODO*) payload);
		break;
		case FIN_LISTA:
			destroy_FIN_LISTA((payload_FIN_LISTA*) payload);
		break;
		case FIN_COMUNICACION:
			destroy_FIN_COMUNICACION((payload_FIN_COMUNICACION*) payload);
		break;
		case ARCHIVO:
			destroy_ARCHIVO((payload_ARCHIVO*) payload);
		break;
		case BLOQUE:
			destroy_BLOQUE((payload_BLOQUE*) payload);
		break;
		case PRESENTACION_DATANODE:
			destroy_PRESENTACION_DATANODE((payload_PRESENTACION_DATANODE*) payload);
		break;
		case JOB:
			destroy_JOB((payload_JOB*) payload);
		break;
		case RESPUESTA_MASTER:
			destroy_RESPUESTA_MASTER((payload_RESPUESTA_MASTER*) payload);
		break;
		case EXITO_OPERACION:
			destroy_EXITO_OPERACION((payload_EXITO_OPERACION*) payload);
		break;
		case FRACASO_OPERACION:
			destroy_FRACASO_OPERACION((payload_FRACASO_OPERACION*) payload);
		break;
		case SCRIPT:
			destroy_SCRIPT((payload_SCRIPT*) payload);
		break;
		case PETICION_BLOQUE:
			destroy_PETICION_BLOQUE((payload_PETICION_BLOQUE*) payload);
		break;
		case UBICACION_BLOQUE:
			destroy_UBICACION_BLOQUE((payload_UBICACION_BLOQUE*) payload);
		break;
		case TEMPORAL:
			destroy_TEMPORAL((payload_TEMPORAL*) payload);
		break;
		case PETICION_TEMPORAL:
			destroy_PETICION_TEMPORAL((payload_PETICION_TEMPORAL*) payload);
		break;
		case RECHAZO_CONEXION:
			destroy_RECHAZO_CONEXION((payload_RECHAZO_CONEXION*) payload);
		break;
		case ALMACENAR_ARCHIVO:
			destroy_ALMACENAR_ARCHIVO((payload_ALMACENAR_ARCHIVO*) payload);
		break;
	}
}

void destroy_SOLICITUD_JOB(payload_SOLICITUD_JOB* payload){
	free(payload->nombreArchivo);
	free(payload);
}

void destroy_ORDEN_TRANSFORMACION(payload_ORDEN_TRANSFORMACION* payload){
	free(payload->nombreArchivoTemporal);
	free(payload);
}

void destroy_ORDEN_REDUCCIONLOCAL(payload_ORDEN_REDUCCIONLOCAL* payload){
	free(payload->nombreTemporal_Transformacion);
	free(payload->nombreTemporal_ReduccionLocal);
	free(payload);
}

void destroy_ORDEN_REDUCCIONGLOBAL(payload_ORDEN_REDUCCIONGLOBAL* payload){
	free(payload->IP_Nodo);
	free(payload->nombreTemporal_ReduccionLocal);
	free(payload->nombreTemporal_ReduccionGlobal);
	free(payload);
}

void destroy_ORDEN_ALMACENAMIENTO(payload_ORDEN_ALMACENAMIENTO* payload){
	free(payload->rutaAlmacenamiento);
	free(payload->nombreAlamcenamiento);
	free(payload->nombreTemporal_ReduccionGlobal);
	free(payload);
}

void destroy_INFO_TRANSFORMACION(payload_INFO_TRANSFORMACION* payload){
	free(payload->IP_Worker);
	free(payload->nombreArchivoTemporal);
	free(payload);
}

void destroy_INFO_REDUCCIONLOCAL(payload_INFO_REDUCCIONLOCAL* payload){
	free(payload->IP_Worker);
	free(payload->nombreTemporal_Transformacion);
	free(payload->nombreTemporal_ReduccionLocal);
	free(payload);
}

void destroy_INFO_REDUCCIONGLOBAL(payload_INFO_REDUCCIONGLOBAL* payload){
	free(payload->IP_Worker);
	free(payload->nombreTemporal_ReduccionLocal);
	free(payload->nombreTemporal_ReduccionGlobal);
	free(payload);
}

void destroy_INFO_ALMACENAMIENTO(payload_INFO_ALMACENAMIENTO* payload){
	free(payload->IP_Worker);
	free(payload->nombreTemporal_ReduccionGlobal);
	free(payload);
}

void destroy_PETICION_NODO(payload_PETICION_NODO* payload){
	free(payload->nombreArchivo);
	free(payload);
}

void destroy_NODO(payload_NODO* payload){
	free(payload->IP_Nodo);
	free(payload->nombreNodo);
	free(payload);
}

void destroy_FIN_LISTA(payload_FIN_LISTA* payload){
	free(payload);
}

void destroy_FIN_COMUNICACION(payload_FIN_COMUNICACION* payload){
	free(payload);
}

void destroy_ARCHIVO(payload_ARCHIVO* payload){
	free(payload->archivo);
	free(payload);
}

void destroy_BLOQUE(payload_BLOQUE* payload){
	free(payload->contenido);
	free(payload);
}

void destroy_PRESENTACION_DATANODE(payload_PRESENTACION_DATANODE* payload){
	free(payload->ipDatanode);
	free(payload);
}

void destroy_JOB(payload_JOB* payload){
	free(payload);
}

void destroy_RESPUESTA_MASTER(payload_RESPUESTA_MASTER* payload){
	free(payload);
}

void destroy_EXITO_OPERACION(payload_EXITO_OPERACION* payload){
	free(payload);
}

void destroy_FRACASO_OPERACION(payload_FRACASO_OPERACION* payload){
	free(payload);
}

void destroy_SCRIPT(payload_SCRIPT* payload){
	free(payload->contenido);
	free(payload);
}

void destroy_PETICION_BLOQUE(payload_PETICION_BLOQUE* payload){
	free(payload);
}

void destroy_UBICACION_BLOQUE(payload_UBICACION_BLOQUE* payload){
	free(payload->ip);
	free(payload);
}

void destroy_TEMPORAL(payload_TEMPORAL* payload){
	free(payload->contenido);
	free(payload);
}

void destroy_PETICION_TEMPORAL(payload_PETICION_TEMPORAL* payload){
	free(payload->nombre);
	free(payload);
}

void destroy_RECHAZO_CONEXION(payload_RECHAZO_CONEXION* payload){
	free(payload);
}

void destroy_ALMACENAR_ARCHIVO(payload_ALMACENAR_ARCHIVO* payload){
	free(payload->contenido);
	free(payload->pathDestino);
	free(payload->nombre);
	free(payload->tipo);
	free(payload);
}

