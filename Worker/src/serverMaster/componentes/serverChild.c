/*
 * init_child.c
 *
 *  Created on: 28/9/2017
 *      Author: utnso
 */
#include "../responses/responses.h"
#include <utilidades/protocolo.h>
#include <utilidades/socket_utils.h>
#include <utilidades/protocol_utils.h>

void init_child(int socket_cliente){
	// Recibir header
	t_Mensaje tipoSolicitud = recibir_header(socket_cliente);
	// Responder de acuerdo al header recibido
	responder_solicitud(tipoSolicitud);
}
