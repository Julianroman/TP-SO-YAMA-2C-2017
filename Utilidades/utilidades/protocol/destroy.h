
#ifndef UTILIDADES_PROTOCOL_DESTROY_H_
#define UTILIDADES_PROTOCOL_DESTROY_H_

#include <stdint.h>

// Destroyer generico
void destroy(HEADER_T header,void* payload);


// Destroyers particulares
void destroy_SOLICITUD_JOB(payload_SOLICITUD_JOB* payload);
void destroy_ORDEN_TRANSFORMACION(payload_ORDEN_TRANSFORMACION* payload);
void destroy_ORDEN_REDUCCIONLOCAL(payload_ORDEN_REDUCCIONLOCAL* payload);
void destroy_ORDEN_REDUCCIONGLOBAL(payload_ORDEN_REDUCCIONGLOBAL* payload);
void destroy_ORDEN_ALMACENAMIENTO(payload_ORDEN_ALMACENAMIENTO* payload);
void destroy_INFO_TRANSFORMACION(payload_INFO_TRANSFORMACION* payload);
void destroy_INFO_REDUCCIONLOCAL(payload_INFO_REDUCCIONLOCAL* payload);
void destroy_INFO_REDUCCIONGLOBAL(payload_INFO_REDUCCIONGLOBAL* payload);
void destroy_INFO_ALMACENAMIENTO(payload_INFO_ALMACENAMIENTO* payload);
void destroy_PETICION_NODO(payload_PETICION_NODO* payload);
void destroy_NODO(payload_NODO* payload);
void destroy_FIN_LISTA(payload_FIN_LISTA* payload);
void destroy_FIN_COMUNICACION(payload_FIN_COMUNICACION* payload);
void destroy_ARCHIVO(payload_ARCHIVO* payload);
void destroy_BLOQUE(payload_BLOQUE* payload);
void destroy_PRESENTACION_DATANODE(payload_PRESENTACION_DATANODE* payload);
void destroy_JOB(payload_JOB* payload);
void destroy_RESPUESTA_MASTER(payload_RESPUESTA_MASTER* payload);
void destroy_EXITO_OPERACION(payload_EXITO_OPERACION* payload);
void destroy_FRACASO_OPERACION(payload_FRACASO_OPERACION* payload);
void destroy_SCRIPT(payload_SCRIPT* payload);
void destroy_PETICION_BLOQUE(payload_PETICION_BLOQUE* payload);
void destroy_UBICACION_BLOQUE(payload_UBICACION_BLOQUE* payload);
void destroy_TEMPORAL(payload_TEMPORAL* payload);
void destroy_PETICION_TEMPORAL(payload_PETICION_TEMPORAL* payload);
void destroy_RECHAZO_CONEXION(payload_RECHAZO_CONEXION* payload);
void destroy_ALMACENAR_ARCHIVO(payload_ALMACENAR_ARCHIVO* payload);

#endif /* UTILIDADES_PROTOCOL_DESTROY_H_ */
