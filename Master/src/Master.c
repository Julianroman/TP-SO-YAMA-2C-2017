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

#define BACKLOGTRANS 12

sem_t recepcionSem;
sem_t balancer;

t_log* logger;

int operaciones;
int masterID;

char* rutaTransformador;
char* rutaReductor;

char* rutaAlmacenado;
char* nombreAlmacenado;

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

sem_t fin_job;

int main(int argc, char **argv) {
	// Recibir parametros
	if (argc!= 4){
		puts("Accion incorrecta, debe ser: Master <archivo yamafs> <transformador> <reductor>");
		return 1;
	}

	// Inicializar semaforo de fin de job
	sem_init(&fin_job, 0, 0);
	sem_init(&recepcionSem, 0, 0);


	// Imprimir titulo
	tituloMaster();

	// Iniciar timer
	time_t inicioJob,finJob;
	time (&inicioJob);


	// Abrir archivos
	rutaTransformador = argv[2];
	rutaReductor      = argv[3];

	// Inicializar logs
	logger = log_create("master.log", "Master", true, LOG_LEVEL_TRACE);
	log_info(logger, "Comienza proceso Master");


	// Leer configuracion
	int puertoYama;
	char* ipYama;
	int balanceo;
	leerConfiguracion("master-config.cfg", &ipYama,&puertoYama,&balanceo);
	log_trace(logger, "Configuracion leida");
	sem_init(&balancer, 0, balanceo);

	// Conectarse al YAMA
	int socketYAMA = crear_conexion(ipYama,puertoYama);
	if(socketYAMA != -1){
		log_info(logger, "Conectado al administrador en %s:%d / socket:%d",ipYama,puertoYama,socketYAMA);
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
			case FIN_COMUNICACION:
				log_error(logger, "El administrador de ha desconectado, Cerrando master ...\n");
				exit(1);
				break;
			default:
				log_warning(logger, "No se reconoce la instruccion del administrador.\n");
				break;
		}

	}

	// Parar timer de job

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


