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
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <utilidades/socket_utils.h>
#include <commons/collections/list.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>

#include "serverWorker/serverWorker.h"

#define PUERTOESCUCHA 8085


t_log* logger;
char* nodePath = "data.bin";

int main(int argc, char **argv) {
	/*if (argc!=2){
		puts("Ingrese un puerto");
		return 1;
	}
	char* puertoString = argv[1];
	int puerto = atoi(puertoString);*/


	// Crear la carpete scripts (si no existe)
	struct stat st = {0};

	if (stat("scripts", &st) == -1) {
		mkdir("scripts", 0700);
	}

	// Manejo de logs
	logger = log_create("worker.log", "Worker", true, LOG_LEVEL_TRACE);

	int puerto = 9095;
	log_trace(logger, "Comenzando Worker en el puerto %d\n",puerto);



	// Servidor de Master
	init_serverMaster(puerto);

	return EXIT_SUCCESS;
}
