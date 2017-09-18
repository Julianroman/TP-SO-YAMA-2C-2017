/*
 ============================================================================
 Name        : Worker.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <parser/metadata_program.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <commons/string.h>
#include <string.h>
#include <unistd.h>
#include <utilidades/socket_utils.h>
#include "serverMaster.h"


t_log* log;

int main(void) {
	puts("Comienza Worker");

	// Manejo de logs
	log = log_create("worker.log", "Worker", false, LOG_LEVEL_TRACE);
	//log_trace(log, "MENSAJE");

	// Servidor
	init_serverMaster();


	return EXIT_SUCCESS;
}
