/*
 * unpackers.h
 * Generado automaticamente por ProtoCool
*/

#ifndef UTILIDADES_PROTOCOL_UNPACKERS_H_
#define UTILIDADES_PROTOCOL_UNPACKERS_H_

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "types.h"

void* unpack_SOLICITUD_PROCESAMIENTO(int socket);
void* unpack_SOLICITUD_REDUCCIONLOCAL(int socket);
void* unpack_SOLICITUD_REDUCCIONGLOBAL(int socket);
void* unpack_SOLICITUD_ALMACENAMIENTO(int socket);
void* unpack_ORDEN_TRANSFORMACION(int socket);
void* unpack_ORDEN_REDUCCIONLOCAL(int socket);
void* unpack_ORDEN_REDUCCIONGLOBAL(int socket);
void* unpack_ORDEN_ALMACENAMIENTO(int socket);
void* unpack_INFO_TRANSFORMACION(int socket);
void* unpack_INFO_REDUCCIONLOCAL(int socket);
void* unpack_INFO_REDUCCIONGLOBAL(int socket);
void* unpack_INFO_ALMACENAMIENTO(int socket);
void* unpack_PETICION_NODO(int socket);
void* unpack_NODO(int socket);
void* unpack_FIN_LISTA(int socket);
void* unpack_FIN_COMUNICACION(int socket);
void* unpack_ARCHIVO(int socket);

#endif /* UTILIDADES_PROTOCOL_UNPACKERS_H_ */
