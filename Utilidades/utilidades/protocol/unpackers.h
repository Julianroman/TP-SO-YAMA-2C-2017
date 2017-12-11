
#ifndef UTILIDADES_PROTOCOL_UNPACKERS_H_
#define UTILIDADES_PROTOCOL_UNPACKERS_H_

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "types.h"

void* unpack_SOLICITUD_JOB(int socket);
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
void* unpack_BLOQUE(int socket);
void* unpack_PRESENTACION_DATANODE(int socket);
void* unpack_JOB(int socket);
void* unpack_RESPUESTA_MASTER(int socket);
void* unpack_EXITO_OPERACION(int socket);
void* unpack_FRACASO_OPERACION(int socket);
void* unpack_SCRIPT(int socket);
void* unpack_PETICION_BLOQUE(int socket);
void* unpack_UBICACION_BLOQUE(int socket);
void* unpack_TEMPORAL(int socket);
void* unpack_PETICION_TEMPORAL(int socket);
void* unpack_RECHAZO_CONEXION(int socket);
void* unpack_ALMACENAR_ARCHIVO(int socket);

#endif /* UTILIDADES_PROTOCOL_UNPACKERS_H_ */
