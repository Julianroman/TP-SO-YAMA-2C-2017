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
	//payload_ORDEN_REDUCCIONLOCAL* payload;
	HEADER_T cabecera;

	/*payload = receive(socket_cliente,&cabecera);
	if(cabecera != ARCHIVO){
		log_error(logger,"Se esperaba un archivo");
	}else{
		log_info(logger,"Archivo recibido");
	}*/
	send_EXITO_OPERACION(socket_cliente);
};

