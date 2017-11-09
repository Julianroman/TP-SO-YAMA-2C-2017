/*
 * ============================================================================
 * res_orden_transformacion.c
 * ============================================================================
 */

#include <stdio.h>
#include <utilidades/protocol/senders.h>
#include <utilidades/protocol/types.h>
#include <utilidades/protocol/receive.h>
#include <commons/log.h>

extern t_log* logger;

void res_ORDEN_TRANSFORMACION(int socket_cliente,HEADER_T header,void* data){
	log_info(logger, "Respondiendo ORDEN_TRANSFORMACION");
	payload_ORDEN_TRANSFORMACION* payload;
	HEADER_T cabecera;

	payload = receive(socket_cliente,&cabecera);
	if(cabecera != ARCHIVO){
		log_error(logger,"Se esperaba un archivo");
	}else{
		log_info(logger,"Transformador recibido");
	}
	send_EXITO_OPERACION(socket_cliente);

};

