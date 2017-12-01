/*
 ============================================================================
 Name        : YAMA.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "serverYAMA/responses.h"
#include "serverYAMA/serverYAMA.h"
#include "YAMA.h"


void leerConfiguracion(){
	char* path = "/home/utnso/tp-2017-2c-Grupo-1---K3525/YAMA/src/yama-config.cfg";
	t_config* archivo_configuracion = config_create(path);
	configYAMA = malloc(sizeof(t_yama));
	configYAMA->FS_PUERTO = config_get_int_value(archivo_configuracion, "FS_PUERTO");
	printf("El puerto FS es: %d \n", configYAMA->FS_PUERTO);

	configYAMA->FS_IP = config_get_string_value(archivo_configuracion, "FS_IP");
	printf("La IP FS es: %s \n", configYAMA->FS_IP);

	configYAMA->retardoPlanificacion = config_get_int_value(archivo_configuracion, "RETARDO_PLANIFICACION");
	printf("El retardo de la Planificacion es: %i \n", configYAMA->retardoPlanificacion);

	configYAMA->algoritmoBalanceo = config_get_string_value(archivo_configuracion, "ALGORITMO_BALANCEO");
	printf("El Algoritmo de Balanceo es: %s \n", configYAMA->algoritmoBalanceo);

	configYAMA->ipYAMA = config_get_string_value(archivo_configuracion, "YAMA_IP");
	printf("YAMA se conectara la IP: %s \n", configYAMA->ipYAMA);

	configYAMA->puertoYAMA = config_get_int_value(archivo_configuracion, "YAMA_PUERTO");
	printf("YAMA se conectara en el puerto: %d \n", configYAMA->puertoYAMA);

	configYAMA->base = config_get_int_value(archivo_configuracion, "BASE");
	printf("La disponibilidad base es: %d \n", configYAMA->base);
}

void crearLog(){
	logYAMA = log_create("yama.log", "YAMA", true, LOG_LEVEL_TRACE);
}

int main(void) {
	puts("Comienza el proceso YAMA");
	crearLog();
	log_trace(logYAMA, "Leyendo configuracion");
	leerConfiguracion();
	log_trace(logYAMA, "Configuracion leida");
	TablaEstados = list_create();
	MastersJobs = list_create();

	log_trace(logYAMA, "Iniciando servidor YAMA");
	init_serverYAMA(configYAMA->puertoYAMA);

	return EXIT_SUCCESS;
}
