/*
 * YAMA.h
 *
 *  Created on: 20/10/2017
 *      Author: utnso
 */

#ifndef YAMA_H_
#define YAMA_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include <commons/config.h>
#include <commons/string.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <utilidades/protocol/receive.h>
#include <utilidades/protocol/types.h>

// Tipo de dato de uso interno del yama
// las respuestas deberan retornar un YAMA_STATUS
// para poder tomar las medidas necesarias en caso de error
typedef enum {ERROR, EXITO, EN_EJECUCION} YAMA_STATUS;
typedef enum {TRANSFORMACION, REDUCCION_LOCAL, REDUCCION_GLOBAL, ALMACENAMIENTO} Tarea;

typedef struct{
	int id;
	YAMA_STATUS estado;
	Tarea etapa;
}t_job;

typedef struct {
	t_job* job;
	int master_id;
	int master_socket;
}t_job_master;

typedef struct{
	int bloqueNodo;
	int bloqueArchivo;
	int copia;
} t_infoBloque;

typedef struct {
	int id;
	char *ip;
	int puerto;
	int carga;
	t_list* infoBloques;
	t_list* bloquesAEjecutar;
	int disponibilidad;
	int cantTareasHistoricas;
	int activo;
	Tarea etapaActiva;
} t_worker;

typedef struct {
	t_job* job;
	int master;
	t_worker* nodo;
	int bloque;
	Tarea tarea;
	char* archivoTemporal;
	YAMA_STATUS estado;
} t_tablaEstados;

typedef struct {
	int puertoFs;
	char* ipFs;
	int puertoYAMA;
	char* ipYAMA;
	int retardoPlanificacion; // En mili segundos
	char* algoritmoBalanceo;
} t_yama;

void iniciarListaEstados();

static t_list* TablaEstados;
t_list* MastersJobs;
t_yama* leerConfiguracion();

t_yama* configYAMA;
t_log* logYAMA;
#endif /* YAMA_H_ */
