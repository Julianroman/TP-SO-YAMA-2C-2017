/*
 * ============================================================================
 * res_orden_almacenamiento.c
 * ============================================================================
 */

#include <stdio.h>
#include <utilidades/protocol/senders.h>
#include <utilidades/protocol/types.h>
#include <utilidades/protocol/receive.h>
#include <commons/log.h>

extern t_log* logger;

void res_ORDEN_ALMACENAMIENTO(int socket_cliente,HEADER_T header,void* data){
	log_info(logger, "Respondiendo ORDEN_ALMACENAMIENTO");
	send_EXITO_OPERACION(socket_cliente);
};
