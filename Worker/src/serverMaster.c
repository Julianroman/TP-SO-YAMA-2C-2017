/*
 * ============================================================================
 * serverMaster.c
 * ============================================================================
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <utilidades/socket_utils.h>
#include <utilidades/protocol_utils.h>
#include <utilidades/protocolo.h>
#include "responses.h"

#define BACKLOG       5


void init_serverMaster(puertoEscucha){
	// Recibir conexion
	int socket_listener = crear_listener(puertoEscucha);
	int socket_cliente = escuchar_socket(socket_listener,BACKLOG);

	while(1){
		/* Ciclo Escuha-Respuesta */
		// En cuanto se incorporen multiples Masters
		// esta estructura debe pasar a ser un multiplexor

		// Recibir header
		t_Mensaje tipoSolicitud = recibir_header(socket_cliente);
		// Responder de acuerdo al header recibido
		responder_solicitud(tipoSolicitud);
	}
};
