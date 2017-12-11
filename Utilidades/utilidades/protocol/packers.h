
#ifndef UTILIDADES_PROTOCOL_UNPACKERS_H_
#define UTILIDADES_PROTOCOL_UNPACKERS_H_

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "types.h"

char* pack_SOLICITUD_JOB(payload_SOLICITUD_JOB payload,int* tamanio_paquete);
char* pack_ORDEN_TRANSFORMACION(payload_ORDEN_TRANSFORMACION payload,int* tamanio_paquete);
char* pack_ORDEN_REDUCCIONLOCAL(payload_ORDEN_REDUCCIONLOCAL payload,int* tamanio_paquete);
char* pack_ORDEN_REDUCCIONGLOBAL(payload_ORDEN_REDUCCIONGLOBAL payload,int* tamanio_paquete);
char* pack_ORDEN_ALMACENAMIENTO(payload_ORDEN_ALMACENAMIENTO payload,int* tamanio_paquete);
char* pack_INFO_TRANSFORMACION(payload_INFO_TRANSFORMACION payload,int* tamanio_paquete);
char* pack_INFO_REDUCCIONLOCAL(payload_INFO_REDUCCIONLOCAL payload,int* tamanio_paquete);
char* pack_INFO_REDUCCIONGLOBAL(payload_INFO_REDUCCIONGLOBAL payload,int* tamanio_paquete);
char* pack_INFO_ALMACENAMIENTO(payload_INFO_ALMACENAMIENTO payload,int* tamanio_paquete);
char* pack_PETICION_NODO(payload_PETICION_NODO payload,int* tamanio_paquete);
char* pack_NODO(payload_NODO payload,int* tamanio_paquete);
char* pack_FIN_LISTA(payload_FIN_LISTA payload,int* tamanio_paquete);
char* pack_FIN_COMUNICACION(payload_FIN_COMUNICACION payload,int* tamanio_paquete);
char* pack_ARCHIVO(payload_ARCHIVO payload,int* tamanio_paquete);
char* pack_BLOQUE(payload_BLOQUE payload,int* tamanio_paquete);
char* pack_PRESENTACION_DATANODE(payload_PRESENTACION_DATANODE payload,int* tamanio_paquete);
char* pack_JOB(payload_JOB payload,int* tamanio_paquete);
char* pack_RESPUESTA_MASTER(payload_RESPUESTA_MASTER payload,int* tamanio_paquete);
char* pack_EXITO_OPERACION(payload_EXITO_OPERACION payload,int* tamanio_paquete);
char* pack_FRACASO_OPERACION(payload_FRACASO_OPERACION payload,int* tamanio_paquete);
char* pack_SCRIPT(payload_SCRIPT payload,int* tamanio_paquete);
char* pack_PETICION_BLOQUE(payload_PETICION_BLOQUE payload,int* tamanio_paquete);
char* pack_UBICACION_BLOQUE(payload_UBICACION_BLOQUE payload,int* tamanio_paquete);
char* pack_TEMPORAL(payload_TEMPORAL payload,int* tamanio_paquete);
char* pack_PETICION_TEMPORAL(payload_PETICION_TEMPORAL payload,int* tamanio_paquete);
char* pack_RECHAZO_CONEXION(payload_RECHAZO_CONEXION payload,int* tamanio_paquete);
char* pack_ALMACENAR_ARCHIVO(payload_ALMACENAR_ARCHIVO payload,int* tamanio_paquete);

#endif /* UTILIDADES_PROTOCOL_UNPACKERS_H_ */
