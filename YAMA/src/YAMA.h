/*
 * YAMA.h
 *
 *  Created on: 20/10/2017
 *      Author: utnso
 */

#ifndef YAMA_H_
#define YAMA_H_

typedef struct {

	char *nombre;
	char *ip;
	char *puerto;
	int carga;
	t_list bloque;
	int disponibilidad;

} t_nodo;

typedef struct {
	t_job job;
	int master;
	t_nodo nodo;
	int bloque;
	char* etapa;
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

#endif /* YAMA_H_ */
