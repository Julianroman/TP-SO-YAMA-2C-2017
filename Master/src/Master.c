/*
 ============================================================================
 Name        : Master.c
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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

#include <utilidades/socket_utils.h>
#include <utilidades/protocol/senders.h>
#include <utilidades/protocol/receive.h>
#include "operaciones/operaciones.h"
// Eliminar define en cuanto la funcion log devuelva la ip
#define IPYAMA "127.0.0.1"

int puertoYama = 0;
char* ipYama = "";
t_log* logger;
sem_t threadManager;

void leerConfiguracion(){
	char* path = "/home/utnso/workspace/tp-2017-2c-Grupo-1---K3525/Master/src/master-config.cfg";
	t_config* archivo_configuracion = config_create(path);
	puertoYama = config_get_int_value(archivo_configuracion, "YAMA_PUERTO");
	printf("El puerto YAMA es: %i \n", puertoYama);
	ipYama = config_get_string_value(archivo_configuracion, "YAMA_IP");
	printf("La IP YAMA es: %s \n", ipYama);
	config_destroy(archivo_configuracion);
}


int main(int argc, char **argv) {
	// TODO Recibir todos los parametros
	// Recibir parametros
	if (argc!=2){
		puts("Ingrese la ruta de un archivo");
		return 1;
	}
	sem_init(&threadManager,0,0);
	char* ruta_yamafs = argv[1];

	// Abrir archivo
	//int transformador_fd = open(rutaTransformador,NULL);


	// Manejo de logs
	logger = log_create("master.log", "Master", true, LOG_LEVEL_TRACE);
	log_trace(logger, "Comienza proceso Master");
	leerConfiguracion();
	log_trace(logger, "Configuracion leida");

	// Conectarse al YAMA
	int socketYAMA = crear_conexion(IPYAMA,puertoYama);
	log_trace(logger, "Conectado al YAMA en socket %d",socketYAMA);

	// Enviar solicitud de procesamiento
	send_SOLICITUD_PROCESAMIENTO(socketYAMA,ruta_yamafs);


	// Escuchar Informaciones
	STATUS_MASTER status;
	HEADER_T header;
	void *data;
	data = receive(socketYAMA,&header);
	while(header != FIN_COMUNICACION){
		switch (header){
			case INFO_TRANSFORMACION:
				status = transformacion (socketYAMA, data);
			break;
			case INFO_REDUCCIONLOCAL:
				status = reduccionLocal (socketYAMA, data);
			break;
			case INFO_REDUCCIONGLOBAL:
				status = reduccionGlobal(socketYAMA, data);
			break;
			case INFO_ALMACENAMIENTO:
				status = almacenamiento (socketYAMA, data);
			break;
			default:
				log_warning(logger,"No se conoce el mensaje enviado por el YAMA");
			break;
		}
		data = receive(socketYAMA,&header);
	}

	// TODO Mostrar estadisticas

	printf("Presione INTRO para terminar...\n");
	getchar();
	return EXIT_SUCCESS;
}
