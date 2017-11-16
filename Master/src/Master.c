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
#include "funcionesMaster.h"

int puertoYama = 0;
char* ipYama = "";
t_log* logger;

int operaciones;
int reduxLocalesRestantes;
int masterID;

char* scriptTransformador;
char* scriptReductor;

int transformacionesEnProceso;
int reduxLocalesEnProceso;
int paralelasEnProceso;

int maxTransformacionesEnProceso = 0;
int maxReduxLocalesEnProceso = 0;
int maxParalelasEnProceso = 0;

int transformacionesRealizadas;
int reduxLocalesRealizadas;

int fallosTransformacion;
int fallosReduxLocal;
int fallosReduxGlobal;
int fallosAlmacenamiento;

double tiempoTransformacion;
double tiempoReduxLocal;
double tiempoReduxGlobal;
double tiempoAlmacenamiento;

int main(int argc, char **argv) {
	// Recibir parametros
	if (argc!= 4){
		puts("Accion incorrecta, debe ser: Master <archivo yamafs> <transformador> <reductor>");
		return 1;
	}


	// Imprimir titulo
	tituloMaster();

	// Iniciar timer
	time_t inicioJob,finJob;
	time (&inicioJob);


	// Abrir archivos
	char* ruta_transformador = argv[2];
	char* ruta_reductor      = argv[3];
	scriptTransformador = scriptToChar(ruta_transformador);
	scriptReductor = scriptToChar(ruta_reductor);


	// Inicializar logs
	logger = log_create("master.log", "Master", true, LOG_LEVEL_TRACE);
	log_trace(logger, "Comienza proceso Master");


	// Leer configuracion
	//char* path = ;
	leerConfiguracion("/home/utnso/workspace/tp-2017-2c-Grupo-1---K3525/Master/src/master-config.cfg", &ipYama,&puertoYama);
	log_trace(logger, "Configuracion leida");

	// Conectarse al YAMA
	int socketYAMA = crear_conexion(ipYama,puertoYama);
	if(socketYAMA != -1){
		log_trace(logger, "Conectado al administrador en %s:%d / socket:%d",ipYama,puertoYama,socketYAMA);
	}else{
		log_error(logger, "No se pudo conectar al administrador.\n");
		return EXIT_FAILURE;
	}

	// Enviar solicitud de procesamiento
	char* ruta_yamafs = argv[1];
	send_SOLICITUD_JOB(socketYAMA,ruta_yamafs);

	// Recibir Job
	recibirJob(socketYAMA,&masterID);

	// Escuchar al administrador
	int jobTerminado = 0;
	HEADER_T header;
	void *data;

	while(jobTerminado == 0){
		data = receive(socketYAMA,&header);

		switch (header) {
			case INFO_TRANSFORMACION:
				transformacion (socketYAMA, data);
				break;
			case INFO_REDUCCIONLOCAL:
				reduccionLocal (socketYAMA, data);
				break;
			case INFO_REDUCCIONGLOBAL:
				reduccionGlobal (socketYAMA, data);
				break;
			case INFO_ALMACENAMIENTO:
				almacenamiento (socketYAMA, data);
				jobTerminado = 1;
				break;
			default:
				log_warning(logger, "No se reconoce la instruccion del administrador.\n");
				break;
		}

	}

	// Parar timer de job
	sleep(1);
	time (&finJob);
	// Mostrar estadisticas
	printf("\x1b[33mEstadisticas de ejecucion\n\n");
	printf("Duracion total                                            : %.2lf segundos\n",difftime(finJob,inicioJob));
	printf("Duracion promedio de transformacion                       : %.2lf segundos.\n",tiempoTransformacion/(transformacionesRealizadas-fallosTransformacion));
	printf("Duracion promedio de reduccion local                      : %.2lf segundos.\n",tiempoReduxLocal/(reduxLocalesRealizadas-fallosReduxLocal));
	printf("Duracion de reduccion global                              : %.2lf segundos.\n",tiempoReduxGlobal);
	printf("Duracion de almacenamiento                                : %.2lf segundos.\n",tiempoAlmacenamiento);
	printf("Cantidad de transformaciones                              : %d transformaciones.\n",transformacionesRealizadas);
	printf("Fallos de transformaciones                                : %d fallas.\n",fallosTransformacion);
	printf("Cantidad de reducciones locales                           : %d reducciones.\n",reduxLocalesRealizadas);
	printf("Fallos de reducciones locales                             : %d fallas.\n",fallosReduxLocal);
	printf("Maxima cantidad de transformaciones\ncorriendo en paralelo                                     : %d transformaciones.\n",maxTransformacionesEnProceso);
	printf("Maxima cantidad de reducciones locales\ncorriendo en paralelo                                     : %d reducciones.\n",maxReduxLocalesEnProceso);
	printf("Maxima cantidad de reducciones locales y transformaciones\ncorriendo en paralelo                                     : %d operaciones.\n",maxParalelasEnProceso);

	printf("\nPresione INTRO para terminar...\n \x1b[0m");
	getchar();

	// Cerrar
	free(ipYama);
	return EXIT_SUCCESS;
}


