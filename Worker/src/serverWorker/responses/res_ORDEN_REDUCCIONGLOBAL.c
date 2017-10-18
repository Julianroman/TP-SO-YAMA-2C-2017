/*
 * ============================================================================
 * res_orden_reduxGlobal.c
 * ============================================================================
 */

#include <stdio.h>
#include <utilidades/protocol/senders.h>
#include <utilidades/protocol/types.h>
#include <utilidades/protocol/receive.h>

void res_ORDEN_REDUCCIONGLOBAL(int socket_cliente,HEADER_T header,void* data){
	printf("Orden de reduccion global recibida");
};
