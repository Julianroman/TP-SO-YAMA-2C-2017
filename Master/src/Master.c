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
#include <pthread.h>

#include <utilidades/socket_utils.h>
#include <utilidades/protocol/senders.h>
#include <utilidades/protocol/receive.h>
#include <utilidades/protocol/types.h>

#include "operaciones/operaciones.h"

// Eliminar define en cuanto la funcion log devuelva la ip
#define IPYAMA "127.0.0.1"

int puertoYama = 0;
char* ipYama = "";
t_log* logger;
sem_t reductionThreads;
pthread_mutex_t transformationManager = PTHREAD_MUTEX_INITIALIZER;

int operaciones;
int reduccionesLocalesRestantes;
int transformacionesRestantes;
int masterID;
int socketYAMA;

int transformador_fd;
int reductor_fd;


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
	printf("La IP YAMA es: %s \n", ipYama);
	// TODO Recibir todos los parametros
	// Recibir parametros
	if (argc!= 4){
		puts("Accion incorrecta, debe ser: Master <archivo yamafs> <transformador> <reductor>");
		return 1;
	}
	char* ruta_yamafs          = argv[1];
	//char* ruta_transformador = argv[2];
	//char* ruta_reductor      = argv[3];

	// Abrir archivos
	//int transformador_fd = open(ruta_transformador,NULL);
	//int reductor_fd = open(ruta_reductor,NULL);

	sem_init(&reductionThreads, 0, 0);

	// Manejo de logs
	logger = log_create("master.log", "Master", true, LOG_LEVEL_TRACE);
	log_trace(logger, "Comienza proceso Master");
	leerConfiguracion();
	log_trace(logger, "Configuracion leida");

	// Conectarse al YAMA
	socketYAMA = crear_conexion(IPYAMA,puertoYama);
	log_trace(logger, "Conectado al YAMA en socket %d",socketYAMA);

	// Enviar solicitud de procesamiento
	send_SOLICITUD_JOB(socketYAMA,ruta_yamafs);

	//Inicializar
	inicializacion();

	// Escuchar Informaciones
	HEADER_T header;
	void *data;

	data = receive(socketYAMA,&header);
	while(header != INFO_REDUCCIONGLOBAL){

		if(header==INFO_TRANSFORMACION) transformacion (socketYAMA, data);
		if(header==INFO_REDUCCIONLOCAL)	reduccionLocal (socketYAMA, data);
		data = receive(socketYAMA,&header);
	}

	// Esperar a todos los hilos de reduccion
	// antes de avanzar
	int i;
	for(i=0;i<operaciones;i++){
		sem_wait(&reductionThreads);
	}

	if(header == INFO_REDUCCIONGLOBAL){
		reduccionGlobal (socketYAMA, data);
	}
	data = receive(socketYAMA,&header);
	if(header == INFO_ALMACENAMIENTO){
		almacenamiento (socketYAMA, data);
	}


	data = receive(socketYAMA,&header);
	// TODO Mostrar estadisticas

	printf("Presione INTRO para terminar...\n");
	getchar();
	close(transformador_fd);
	close(reductor_fd);
	return EXIT_SUCCESS;
}

