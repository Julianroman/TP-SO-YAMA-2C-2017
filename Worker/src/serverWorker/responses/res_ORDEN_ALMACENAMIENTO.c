/*
 * ============================================================================
 * res_orden_almacenamiento.c
 * ============================================================================
 */
#include <stdio.h>
#include <utilidades/protocol/senders.h>
#include <utilidades/protocol/types.h>
#include <utilidades/protocol/receive.h>

void res_ORDEN_ALMACENAMIENTO(int socket_cliente,HEADER_T header,void* data){
	printf("Orden de almacenamiento recibida");
};
