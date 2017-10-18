/*
 * ============================================================================
 * serverMaster.c
 * ============================================================================
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>

#include <utilidades/socket_utils.h>

#include "../serverWorker/serverChild.h"

#define BACKLOG       5


void init_serverMaster(int puertoEscucha, t_log* logger){
	log_trace(logger, "Esperando instrucciones");
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
			puts("Proceso creado");
			//Hijo
			init_child(socket_cliente);  // una funcion que dada la nueva conexion,
										 // se encargara de responder al master
			exit(0);
		}
		if (pid > 0){
			log_trace(logger, "Proceso %d creado",pid);
			//Padre
			close(socket_cliente);
			socket_cliente = escuchar_socket(socket_listener,BACKLOG);
		}

	}
};
