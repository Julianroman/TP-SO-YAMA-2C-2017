/*
 * ============================================================================
 * serverMaster.c
 * ============================================================================
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


#include <utilidades/socket_utils.h>
#include <utilidades/protocol_utils.h>
#include <utilidades/protocolo.h>
#include "responses/responses.h"
#include "componentes/serverChild.h"

#define BACKLOG       5


void init_serverMaster(int puertoEscucha){
	// Recibir conexion
	int socket_listener = crear_listener(puertoEscucha);
	int socket_cliente = escuchar_socket(socket_listener,BACKLOG);

	while(1){
		pid_t pid = fork();
		if (pid < 0){
			//Error
			printf("Error en fork");
		}
		if (pid == 0){
			//Hijo
			init_child(socket_cliente);  // una funcion que dada la nueva conexion,
										 // se encargara de responder al master
			exit(0);
		}
		//Padre
		socket_cliente = escuchar_socket(socket_listener,BACKLOG);

	}
};
