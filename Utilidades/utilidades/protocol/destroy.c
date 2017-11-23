
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

void destroy(HEADER_T header,void* payload){
	switch(header) {
	case SOLICITUD_JOB:
		free(payload->nombreArchivo);
		free(payload);
	break;
	case ORDEN_TRANSFORMACION:
		free(payload->nombreArchivoTemporal);
		free(payload);
	break;
	case ORDEN_REDUCCIONLOCAL:
		free(payload->nombreTemporal_Transformacion);
		free(payload->nombreTemporal_ReduccionLocal);
		free(payload);
	break;
	case ORDEN_REDUCCIONGLOBAL:
		free(payload->IP_Nodo);
		free(payload->nombreTemporal_ReduccionLocal);
		free(payload->nombreTemporal_ReduccionGlobal);
		free(payload);
	break;
	case ORDEN_ALMACENAMIENTO:
		free(payload->nombreTemporal_ReduccionGlobal);
		free(payload);
	break;
	case INFO_TRANSFORMACION:
		free(payload->IP_Worker);
		free(payload->nombreArchivoTemporal);
		free(payload);
	break;
	case INFO_REDUCCIONLOCAL:
		free(payload->IP_Worker);
		free(payload->nombreTemporal_Transformacion);
		free(payload->nombreTemporal_ReduccionLocal);
		free(payload);
	break;
	case INFO_REDUCCIONGLOBAL:
		free(payload->IP_Worker);
		free(payload->nombreTemporal_ReduccionLocal);
		free(payload->nombreTemporal_ReduccionGlobal);
		free(payload);
	break;
	case INFO_ALMACENAMIENTO:
		free(payload->IP_Worker);
		free(payload->nombreTemporal_ReduccionGlobal);
		free(payload);
	break;
	case PETICION_NODO:
		free(payload->nombreArchivo);
		free(payload);
	break;
	case NODO:
		free(payload->IP_Nodo);
		free(payload->nombreNodo);
		free(payload);
	break;
	case FIN_LISTA:
		free(payload);
	break;
	case FIN_COMUNICACION:
		free(payload);
	break;
	case ARCHIVO:
		free(payload->archivo);
		free(payload);
	break;
	case BLOQUE:
		free(payload->contenido);
		free(payload);
	break;
	case PRESENTACION_DATANODE:
		free(payload);
	break;
	case JOB:
		free(payload);
	break;
	case RESPUESTA_MASTER:
		free(payload);
	break;
	case EXITO_OPERACION:
		free(payload);
	break;
	case FRACASO_OPERACION:
		free(payload);
	break;
	case SCRIPT:
		free(payload->contenido);
		free(payload);
	break;
	case PETICION_BLOQUE:
		free(payload);
	break;
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

