/*
 * etapa_transformacion.c
 */
#include <stdio.h>
#include <utilidades/protocol/senders.h>
#include <utilidades/protocol/types.h>
#include <utilidades/protocol/receive.h>
#include <commons/log.h>
#include "etapas.h"

MASTER_STATUS etapa_reduccionGlobal (int socketYAMA,t_log* logger){
	log_trace(logger, "Iniciando etapa de reduccion global...");

	send_SOLICITUD_REDUCCIONGLOBAL(socketYAMA);
	HEADER_T header;
	payload_SOLICITUD_REDUCCIONGLOBAL* payload;
	void* data;

	data = receive(socketYAMA,&header);
	if (header == FIN_COMUNICACION){ /*Si header es FIN_COMUNICACION es porque se cerro la conexion */ }

	// Recibir todas las instrucciones
	// Hasta que termine la lista
	while(header != FIN_LISTA){
		// Me aseguro que sean instrucciones de reduccion
		if (header == INFO_REDUCCIONGLOBAL){
			payload = data;
			/*
			 *
			 *
			 * CODIGO DE ACCION
			 *
			 *
			 */
		}
		//
		data = receive(socketYAMA,&header);
		if (header == FIN_COMUNICACION){ /*Si header es FIN_COMUNICACION es porque se cerro la conexion */}
	}
	// IMPORTANTE! HAY UN LEAK DE MEMORIA
	// LA FUNCION DE RECOLECCION ESTA EN DESARROLLO
	// Codigo de transformacion
	return EXITO;
};

