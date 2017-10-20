/*
 * ============================================================================
 * res_orden_reduxLocal.c
 * ============================================================================
 */

#include <stdio.h>
#include <utilidades/protocol/senders.h>
#include <utilidades/protocol/types.h>
#include <utilidades/protocol/receive.h>
#include <commons/log.h>

extern t_log* logger;

void res_ORDEN_REDUCCIONLOCAL(int socket_cliente,HEADER_T header,void* data){
	log_info(logger, "Respondiendo ORDEN_REDUCCIONLOCAL");
	send_FIN_LISTA(socket_cliente);
};

