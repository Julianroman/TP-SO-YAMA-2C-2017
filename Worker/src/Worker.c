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

#include "serverWorker/serverWorker.h"

#define PUERTOESCUCHA 8085


t_log* logger;

int main(int argc, char **argv) {
	/*if (argc!=2){
		puts("Ingrese un puerto");
		return 1;
	}
	char* puertoString = argv[1];
	int puerto = atoi(puertoString);*/

	// Manejo de logs
	logger = log_create("worker.log", "Worker", true, LOG_LEVEL_TRACE);

	int puerto = 9095;
	log_trace(logger, "Comenzando Worker en el puerto %d\n",puerto);



	// Servidor de Master
	init_serverMaster(puerto);

	return EXIT_SUCCESS;
}
