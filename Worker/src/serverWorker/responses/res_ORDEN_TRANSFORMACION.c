/*
 * ============================================================================
 * res_orden_transformacion.c
 * ============================================================================
 */

#include <stdio.h>
#include <utilidades/protocol/senders.h>
#include <utilidades/protocol/types.h>
#include <utilidades/protocol/receive.h>

void res_ORDEN_TRANSFORMACION(int socket_cliente,HEADER_T header,void* data){
	send_FIN_LISTA(socket_cliente);
};

