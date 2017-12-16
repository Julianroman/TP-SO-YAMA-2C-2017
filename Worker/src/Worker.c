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
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>

#include "serverWorker/serverWorker.h"

#define PUERTOESCUCHA 5042

int puertoFS;
char* ipFS;

t_log* logger;
char* nodePath;

void leerConfiguracion(char* path){
	t_config* archivo_configuracion = config_create(path);
	puertoFS = config_get_int_value(archivo_configuracion, "FS_PUERTO");
	ipFS = config_get_string_value(archivo_configuracion, "FS_IP");
	nodePath = config_get_string_value(archivo_configuracion, "RUTA_DATABIN");
	config_destroy(archivo_configuracion);
}

int main(int argc, char **argv) {
	/* -------- DEV-FEATURE ---------------------------------------------- */
	// Opcion de asignar puerto para multiples workers en el mismo ordenador
	int puerto;
	if (argc==2){
		char* puertoString = argv[1];
		puerto = atoi(puertoString);
	} else{
		puerto = PUERTOESCUCHA;
	}
	/* -- END / DEV-FEATURE ---------------------------------------------- */

	// Leer configuracion
	leerConfiguracion("worker-config.cfg");

	// Vaciar scripts
	system("rm -rf scripts/");

	// Creao carpetas (si no existen)
	struct stat st = {0};
	if (stat("scripts", &st) == -1) {
		mkdir("scripts", 0700);
	}
	if (stat("tmp", &st) == -1) {
		mkdir("tmp", 0700);
	}




	// Manejo de logs
	logger = log_create("worker.log", "Worker", true, LOG_LEVEL_TRACE);
	log_trace(logger, "Configuracion leida");

	log_trace(logger, "Comenzando Worker en el puerto %d\n",puerto);



	// Servidor de Master
	init_serverMaster(puerto);

	return EXIT_SUCCESS;
}
