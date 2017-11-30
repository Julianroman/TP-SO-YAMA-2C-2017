/*
 * ============================================================================
 * responses.h
 * ============================================================================
 */

#ifndef RESPONSES_H_
#define RESPONSES_H_

#include <utilidades/protocol/senders.h>
#include <utilidades/protocol/types.h>
#include <utilidades/protocol/receive.h>

// Ejecutor
void responder_ORDENES(int socket_cliente,HEADER_T header,void* data);

// Respuestas
void res_ORDEN_TRANSFORMACION  (int socket_cliente,HEADER_T header,void* data);
void res_ORDEN_REDUCCIONLOCAL  (int socket_cliente,HEADER_T header,void* data);
void res_ORDEN_REDUCCIONGLOBAL (int socket_cliente,HEADER_T header,void* data);
void res_ORDEN_ALMACENAMIENTO  (int socket_cliente,HEADER_T header,void* data);
void res_PETICION_TEMPORAL     (int socket_cliente,HEADER_T header,void* data);

#endif /* RESPONSES_H_ */
