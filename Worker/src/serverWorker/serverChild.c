/*
 * init_child.c
 *
 *  Created on: 28/9/2017
 *      Author: utnso
 */

#include <stdio.h>
#include <utilidades/socket_utils.h>
#include <utilidades/protocol/types.h>
#include <utilidades/protocol/receive.h>
#include <utilidades/protocol/senders.h>
#include "responses/responses.h"

void init_child(int socket_cliente){
	HEADER_T header;
	void* data = receive(socket_cliente,&header);
	if (header == FIN_COMUNICACION){ //Si header es FIN_COMUNICACION es porque se cerro la conexion

	}else{
		responder_ORDENES(socket_cliente,header,data);
	}
}
