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

char* scriptTransformador;
char* scriptReductor;


void leerConfiguracion(){
	char* path = "/home/utnso/workspace/tp-2017-2c-Grupo-1---K3525/Master/src/master-config.cfg";
	t_config* archivo_configuracion = config_create(path);
	puertoYama = config_get_int_value(archivo_configuracion, "YAMA_PUERTO");
	char* pivote = config_get_string_value(archivo_configuracion, "YAMA_IP");
	ipYama = malloc(strlen(pivote)+1);
	strcpy(ipYama, pivote);
	config_destroy(archivo_configuracion);
}

char* scriptToChar(char* path){
	char *file_contents;
	long input_file_size;
	FILE *input_file = fopen(path, "rb");
	fseek(input_file, 0, SEEK_END);
	input_file_size = ftell(input_file);
	rewind(input_file);
	file_contents = malloc(input_file_size * (sizeof(char)));
	fread(file_contents, sizeof(char), input_file_size, input_file);
	fclose(input_file);
	return file_contents;
}

void tituloFancy(){
	printf("\e[1;1H\e[2J");
		puts("\n                   ██╗   ██╗ █████╗ ███╗   ███╗ █████╗ \n                   ╚██╗ ██╔╝██╔══██╗████╗ ████║██╔══██╗\n                    ╚████╔╝ ███████║██╔████╔██║███████║\n                     ╚██╔╝  ██╔══██║██║╚██╔╝██║██╔══██║\n                      ██║   ██║  ██║██║ ╚═╝ ██║██║  ██║\n                      ╚═╝   ╚═╝  ╚═╝╚═╝     ╚═╝╚═╝  ╚═╝\n");
		puts("┬ ┬┌─┐┌┬┐  ┌─┐┌┐┌┌─┐┌┬┐┬ ┬┌─┐┬─┐  ┌┬┐┬─┐  ┌─┐┌┬┐┌┬┐┬┌┐┌┬┌─┐┌┬┐┬─┐┌─┐┌┬┐┌─┐┬─┐\n└┬┘├┤  │   ├─┤││││ │ │ ├─┤├┤ ├┬┘  │││├┬┘  ├─┤ ││││││││││└─┐ │ ├┬┘├─┤ │ │ │├┬┘\n ┴ └─┘ ┴   ┴ ┴┘└┘└─┘ ┴ ┴ ┴└─┘┴└─  ┴ ┴┴└─  ┴ ┴─┴┘┴ ┴┴┘└┘┴└─┘ ┴ ┴└─┴ ┴ ┴ └─┘┴└─");
}


int main(int argc, char **argv) {
	// Recibir parametros
	if (argc!= 4){
		puts("Accion incorrecta, debe ser: Master <archivo yamafs> <transformador> <reductor>");
		return 1;
	}

	// Imprimir titulo
	tituloFancy();

	// Abrir archivos
	char* ruta_yamafs        = argv[1];
	char* ruta_transformador = argv[2];
	char* ruta_reductor      = argv[3];

	scriptTransformador = scriptToChar(ruta_transformador);
	scriptReductor = scriptToChar(ruta_reductor);


	// Inicializar logs
	logger = log_create("master.log", "Master", true, LOG_LEVEL_TRACE);
	log_trace(logger, "Comienza proceso Master");

	sem_init(&reductionThreads, 0, 0);


	// Leer configuracion
	leerConfiguracion();
	log_trace(logger, "Configuracion leida");

	// Conectarse al YAMA
	socketYAMA = crear_conexion(ipYama,puertoYama);
	if(socketYAMA != -1){
		log_trace(logger, "Conectado al administrador en %s:%d / socket:%d",ipYama,puertoYama,socketYAMA);
	}else{
		log_error(logger, "No se pudo conectar al administrador.\n");
		return EXIT_FAILURE;
	}

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

	// TODO Mostrar estadisticas
	printf("\x1b[33mEstadisticas de ejecucion\n\x1b[0m");

	printf("\x1b[33mPresione INTRO para terminar...\n \x1b[0m");
	getchar();

	// Cerranding ...
	free(ipYama);
	return EXIT_SUCCESS;
}


