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


t_yama* leerConfiguracion(){
	char* path = "/home/git/tp-2017-2c-Grupo-1---K3525/YAMA/src/yama-config.cfg";
	t_config* archivo_configuracion = config_create(path);
	t_yama *configYAMA = malloc(sizeof(t_yama));
	configYAMA->puertoFs = config_get_int_value(archivo_configuracion, "FS_PUERTO");
	printf("El puerto FS es: %d \n", configYAMA->puertoFs);
	configYAMA->ipFs = config_get_string_value(archivo_configuracion, "FS_IP");
	printf("La IP FS es: %s \n", configYAMA->ipFs);
	configYAMA->retardoPlanificacion = config_get_int_value(archivo_configuracion, "RETARDO_PLANIFICACION");
	printf("El retardo de la Planificacion es: %i \n", configYAMA->retardoPlanificacion);
	configYAMA->algoritmoBalanceo = config_get_string_value(archivo_configuracion, "ALGORITMO_BALANCEO");
	printf("El Algoritmo de Balanceo es: %s \n", configYAMA->algoritmoBalanceo);
	configYAMA->ipYAMA = config_get_string_value(archivo_configuracion, "YAMA_IP");
	printf("YAMA se conectara la IP: %s \n", configYAMA->ipYAMA);
	configYAMA->puertoYAMA = config_get_int_value(archivo_configuracion, "YAMA_PUERTO");
	printf("YAMA se conectara en el puerto: %d \n", configYAMA->puertoYAMA);


	config_destroy(archivo_configuracion);
	return configYAMA;
}

void crearLog(){
	logYAMA = log_create("yama.log", "YAMA", true, LOG_LEVEL_TRACE);
}

int main(void) {
	puts("Comienza el proceso YAMA");
	crearLog();
	log_trace(logYAMA, "Leyendo configuracion");
	t_yama* configYAMA = leerConfiguracion();
	log_trace(logYAMA, "Configuracion leida");
	TablaEstados = list_create();
	diccionarioMasters = dictionary_create();
	//cliente(ipFs, puertoFs, id);
	log_trace(logYAMA, "Iniciando servidor YAMA");
	init_serverYAMA((int)configYAMA->puertoYAMA);

	return EXIT_SUCCESS;
}
