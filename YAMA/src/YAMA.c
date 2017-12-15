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

int SocketFSGlobal;
sem_t binarioFS;
sem_t binarioSocketFS;

void leerConfiguracion(){
	//char* path = "/home/utnso/tp-2017-2c-Grupo-1---K3525/YAMA/Debug/yama-config.cfg";
	char* path = "yama-config.cfg";
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

int main(void) {
	puts("Comienza el proceso YAMA");
	logYAMA = log_create("yama.log", "YAMA", true, LOG_LEVEL_TRACE);
	log_trace(logYAMA, "Leyendo configuracion");
	leerConfiguracion();
	log_trace(logYAMA, "Configuracion leida");

	TablaEstados = list_create();
	MastersJobs = list_create();

	clienteFS();
	init_serverYAMA(configYAMA->puertoYAMA);

	return EXIT_SUCCESS;
}

void clienteFS(){
	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = inet_addr(configYAMA->FS_IP);
	direccionServidor.sin_port = htons(configYAMA->FS_PUERTO);

	int cliente = socket(AF_INET, SOCK_STREAM, 0);
	SocketFSGlobal = cliente;
	if (connect(cliente, (void*) &direccionServidor, sizeof(direccionServidor)) != 0) {
		perror("No se pudo conectar");
		exit(1);
	}
};
