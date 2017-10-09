/*
 * responder_ORDENES.c
 *
 *  Created on: 17/9/2017
 *      Author: utnso
 */
#include "responses.h"
#include <stdio.h>
void responder_ORDENES(int socket_cliente,HEADER_T tipoDeMensaje,void* data){

	switch (tipoDeMensaje){
	case ORDEN_TRANSFORMACION:
		res_orden_transformacion();
		break;
	case ORDEN_REDUCCIONLOCAL:
		res_orden_reduxLocal();
		break;
	case ORDEN_REDUCCIONGLOBAL:
		res_orden_reduxGlobal();
		break;
	case ORDEN_ALMACENAMIENTO:
		res_orden_almacenamiento();
		break;
	default:
		printf("Mensaje desconocido");
	}
};

