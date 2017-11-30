/*
 * responder_ORDENES.c
 *
 *  Created on: 17/9/2017
 *      Author: utnso
 */
#include "responses.h"
#include <stdio.h>
#include <utilidades/protocol/senders.h>
#include <utilidades/protocol/types.h>
#include <utilidades/protocol/receive.h>
#include <commons/log.h>

extern t_log* logger;

void responder_ORDENES(int socket_cliente,HEADER_T tipoDeMensaje,void* data){

	switch (tipoDeMensaje){
	case ORDEN_TRANSFORMACION:
		res_ORDEN_TRANSFORMACION(socket_cliente,tipoDeMensaje,data);
		break;
	case ORDEN_REDUCCIONLOCAL:
		res_ORDEN_REDUCCIONLOCAL(socket_cliente,tipoDeMensaje,data);
		break;
	case ORDEN_REDUCCIONGLOBAL:
		res_ORDEN_REDUCCIONGLOBAL(socket_cliente,tipoDeMensaje,data);
		break;
	case ORDEN_ALMACENAMIENTO:
		res_ORDEN_ALMACENAMIENTO(socket_cliente,tipoDeMensaje,data);
		break;
	case PETICION_TEMPORAL:
		res_PETICION_TEMPORAL(socket_cliente,tipoDeMensaje,data);
		break;
	default:
		log_warning(logger,"Mensaje desconocido");
	}
};

