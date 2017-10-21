/*
 * YAMA.h
 *
 *  Created on: 20/10/2017
 *      Author: utnso
 */

#ifndef YAMA_H_
#define YAMA_H_

typedef enum {TRANSFORMACION, REDUCCION_LOCAL, REDUCCION_GLOBAL, ALMACENAMIENTO} ETAPA_JOB;

typedef struct {
	t_job job;
	int master;
	t_nodo nodo;
	int bloque;
	ETAPA_JOB etapa;
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

t_yama* leerConfiguracion();
#endif /* YAMA_H_ */
