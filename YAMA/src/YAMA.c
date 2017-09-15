/*
 ============================================================================
 Name        : YAMA.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

//#include "../../Funciones/Sockets.h"
// Probando la shared
#include <utilidades/Sockets.c>
// con la shared no se importan los include de funciones
// asi que los agrego
#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>

int id = 1;
int puertoFs = 0;
char* ipFs = "";
int retardoPlanificacion = 0;
char* algoritmoBalanceo = "";

t_log* log;

void leerConfiguracion(){
	char* path = "/home/utnso/workspace/tp-2017-2c-Grupo-1---K3525/YAMA/src/yama-config.cfg";
	t_config* archivo_configuracion = config_create(path);
	puertoFs = config_get_int_value(archivo_configuracion, "FS_PUERTO");
	printf("El puerto FS es: %i \n", puertoFs);
	ipFs = config_get_string_value(archivo_configuracion, "FS_IP");
	printf("La IP FS es: %s \n", ipFs);
	retardoPlanificacion = config_get_int_value(archivo_configuracion, "RETARDO_PLANIFICACION");
	printf("El retardo de la Planificacion es: %i \n", retardoPlanificacion);
	algoritmoBalanceo = config_get_string_value(archivo_configuracion, "ALGORITMO_BALANCEO");
	printf("El Algoritmo de Balanceo es: %s \n", algoritmoBalanceo);

	config_destroy(archivo_configuracion);
}

int main(void) {
	puts("Comienza el proceso YAMA");

	log = log_create("yama.log", "YAMA", false, LOG_LEVEL_TRACE);

	log_trace(log, "Leyendo configuracion");
	leerConfiguracion();
	log_trace(log, "Configuracion leida");

	cliente(ipFs, puertoFs, id);

	return EXIT_SUCCESS;
}


