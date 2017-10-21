/*
 * YAMA.h
 *
 *  Created on: 20/10/2017
 *      Author: utnso
 */

#ifndef YAMA_H_
#define YAMA_H_

typedef enum {TRANSFORMACION, REDUCCION_LOCAL, REDUCCION_GLOBAL, ALMACENAMIENTO} ETAPA_JOB;
typedef enum {EJECUCION_OK, EJECUCION_ERROR} ESTADO_EJECUCION;

typedef struct {
	t_job* job;
	int master;
	t_nodo* nodo;
	int bloque;
	ETAPA_JOB etapa; // El job tiene una etapa especifica? Debería poder estar en varias a la vez
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
	t_nodo* nodo;
	t_job* jobEjecutado;
	t_tarea* tareaEjecutada;
	ESTADO_EJECUCION estadoEjecucion;
	int master; //Hay un struct de master?
} respuestaInfoMaster;

void iniciarListaEstados();

t_list* TablaEstados;
t_yama* leerConfiguracion();
#endif /* YAMA_H_ */
