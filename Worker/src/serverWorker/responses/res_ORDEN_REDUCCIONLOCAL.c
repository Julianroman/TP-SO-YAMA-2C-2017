/*
 * ============================================================================
 * res_orden_reduxLocal.c
 * ============================================================================
 */

#include <stdio.h>
#include <utilidades/protocol/senders.h>
#include <utilidades/protocol/types.h>
#include <utilidades/protocol/receive.h>

void res_ORDEN_REDUCCIONLOCAL(int socket_cliente,HEADER_T header,void* data){
	printf("Orden de reduccion local recibida");
};

