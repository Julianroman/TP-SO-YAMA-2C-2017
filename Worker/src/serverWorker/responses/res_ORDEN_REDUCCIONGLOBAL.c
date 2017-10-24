/*
 * ============================================================================
 * res_orden_reduxGlobal.c
 * ============================================================================
 */

#include <stdio.h>
#include <utilidades/protocol/senders.h>
#include <utilidades/protocol/types.h>
#include <utilidades/protocol/receive.h>
#include <commons/log.h>

extern t_log* logger;

void res_ORDEN_REDUCCIONGLOBAL(int socket_cliente,HEADER_T header,void* data){
	log_info(logger, "Respondiendo ORDEN_REDUCCIONGLOBAL");
	payload_ORDEN_REDUCCIONGLOBAL* payload = data;

	if (header == FIN_COMUNICACION){ /*Si header es FIN_COMUNICACION es porque se cerro la conexion*/ }

	// Recibir todas las instrucciones
	// Hasta que termine la lista
	while(header != FIN_LISTA){
		if (header == ORDEN_REDUCCIONGLOBAL){
			log_info(logger, "Conectando a Worker en %s:%d / ",payload->IP_Nodo,payload->PUERTO_Nodo);
		}else{
			log_error(logger,"Se esperaba una orden de reduccion global");
		}
		payload = receive(socket_cliente,&header);
		if (header == FIN_COMUNICACION){ /*Si header es FIN_COMUNICACION es porque se cerro la conexion */}
	}
	send_EXITO_OPERACION(socket_cliente);
};
