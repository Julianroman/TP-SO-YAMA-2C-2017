/*
 * etapa_transformacion.c
 */
#include <stdio.h>
#include <utilidades/protocol/senders.h>
#include <utilidades/protocol/types.h>
#include <utilidades/protocol/receive.h>
#include <commons/log.h>
#include "etapas.h"

MASTER_STATUS etapa_almacenamiento (int socketYAMA,t_log* logger){
	log_trace(logger, "Iniciando etapa de almacenamiento...");

	HEADER_T header;
	payload_INFO_ALMACENAMIENTO* payload;
	void* data;

	send_SOLICITUD_ALMACENAMIENTO(socketYAMA);
	data = receive(socketYAMA,&header);

	if (header == FIN_COMUNICACION){ /*Si header es FIN_COMUNICACION es porque se cerro la conexion */ }

	// Recibir todas las instrucciones
	// Hasta que termine la lista
	while(header != FIN_LISTA){
		// Me aseguro que sean instrucciones de transformacion
		if (header == INFO_ALMACENAMIENTO){
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
	puts("Todas las instrucciones fueron recibidas");
	// IMPORTANTE! HAY UN LEAK DE MEMORIA
	// LA FUNCION DE RECOLECCION ESTA EN DESARROLLO
	// Codigo de transformacion
	return EXITO;
};
