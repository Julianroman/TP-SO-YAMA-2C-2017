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
	payload_ORDEN_REDUCCIONGLOBAL* payload;
	HEADER_T cabecera;
	void* paquete;

	paquete = receive(socket_cliente,&cabecera);

	if (header == FIN_COMUNICACION){ /*Si header es FIN_COMUNICACION es porque se cerro la conexion*/ }

	// Recibir todas las instrucciones
	// Hasta que termine la lista
	while(cabecera != FIN_LISTA){
		if (cabecera == ORDEN_REDUCCIONGLOBAL){
			payload = paquete;
			log_info(logger, "Conectando a Worker en %s-->%s / ",payload->nombreTemporal_ReduccionLocal,payload->nombreTemporal_ReduccionGlobal);
		}
		data = receive(socket_cliente,&cabecera);
		if (cabecera == FIN_COMUNICACION){ /*Si header es FIN_COMUNICACION es porque se cerro la conexion */}
	}
	send_FIN_LISTA(socket_cliente);
};
