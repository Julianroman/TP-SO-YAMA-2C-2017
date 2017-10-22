/*
 * YAMA.h
 *
 *  Created on: 20/10/2017
 *      Author: utnso
 */

#ifndef YAMA_H_
#define YAMA_H_

#include <utilidades/Sockets.c>
#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include <commons/string.h>
#include "Job.h"
#include "Tarea.h"

// Tipo de dato de uso interno del yama
// las respuestas deberan retornar un YAMA_STATUS
// para poder tomar las medidas necesarias en caso de error
typedef enum {EXITO, EN_EJECUCION, ERROR} YAMA_STATUS;

typedef enum {TRANSFORMACION, REDUCCION_LOCAL, REDUCCION_GLOBAL, ALMACENAMIENTO} ETAPA_JOB;
typedef enum {EJECUCION_OK, EJECUCION_ERROR} ESTADO_EJECUCION;

typedef struct {

	int id;
	char *ip;
	char *puerto;
	int carga;
	t_list* bloquesDelDatanode;
	t_list* bloquesAEjecutar;
	int disponibilidad;
	int activo;
	int cantTareasHistoricas;
	t_job* jobActivo;

} t_worker;

typedef struct {
	t_job* job;
	int master;
	t_worker* nodo;
	int bloque;
	t_tarea* etapa; // El job tiene una etapa especifica? Debería poder estar en varias a la vez
	char* archivoTemporal;
	YAMA_STATUS estado;
} t_tablaEstados;

typedef struct {
	char* puertoFs;
	char* ipFs;
	char* puertoYAMA;
	char* ipYAMA;
	int retardoPlanificacion;
	char* algoritmoBalanceo;
} t_yama;

typedef struct {
	t_worker* nodo;
	t_job* jobEjecutado;
	t_tarea* tareaEjecutada;
	ESTADO_EJECUCION estadoEjecucion;
	int master; //Hay un struct de master?
} respuestaInfoMaster;

void iniciarListaEstados();

t_list* TablaEstados;
t_yama* leerConfiguracion();
#endif /* YAMA_H_ */
