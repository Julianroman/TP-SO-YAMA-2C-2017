/*
 * serverWorker.c
 *
 *  Created on: 17/9/2017
 *      Author: utnso
 */

#include <commons/collections/dictionary.h>
#include <utilidades/socket_utils.h>
#include <utilidades/protocolo.h>

#include "responses/responses.h"
#include "componentes/multiplexor.h"
#include "serverMaster.h"

#define BACKLOG       5


void init_serverMaster(int puertoEscucha){
	// Diccionario de masters
	t_dictionary* masters_dictionary = dictionary_create();
	// Recibir conexion
	int socket_listener = crear_listener(puertoEscucha);
	// Multiplexar listener
	multiplexar(socket_listener,BACKLOG,&responder_solicitud,masters_dictionary);
};
