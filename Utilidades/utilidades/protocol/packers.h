/*
 * packers.h
 * Generado automaticamente por ProtoCool
*/

#ifndef UTILIDADES_PROTOCOL_UNPACKERS_H_
#define UTILIDADES_PROTOCOL_UNPACKERS_H_

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "types.h"

char* pack_SOLICITUD_PROCESAMIENTO(payload_SOLICITUD_PROCESAMIENTO payload,int* tamanio_paquete);
char* pack_SOLICITUD_REDUCCIONLOCAL(payload_SOLICITUD_REDUCCIONLOCAL payload,int* tamanio_paquete);
char* pack_SOLICITUD_REDUCCIONGLOBAL(payload_SOLICITUD_REDUCCIONGLOBAL payload,int* tamanio_paquete);
char* pack_SOLICITUD_ALMACENAMIENTO(payload_SOLICITUD_ALMACENAMIENTO payload,int* tamanio_paquete);
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

#endif /* UTILIDADES_PROTOCOL_UNPACKERS_H_ */
