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
#include <utilidades/protocol/types.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include <commons/string.h>
#include "Tarea.h"

// Tipo de dato de uso interno del yama
// las respuestas deberan retornar un YAMA_STATUS
// para poder tomar las medidas necesarias en caso de error
typedef enum {ERROR, EXITO, EN_EJECUCION} YAMA_STATUS;
typedef enum {EJECUCION_OK, EJECUCION_ERROR} ESTADO_EJECUCION;

typedef struct{
	uint16_t id;
	YAMA_STATUS estado;
}t_job;

typedef struct {
	int id;
	char *ip;
	int puerto;
	int carga;
	t_list* bloquesDelDatanode;
	t_list* bloquesAEjecutar;
	int disponibilidad;
	int cantTareasHistoricas;
	int activo;
	t_job* jobActivo;
	t_tarea* tareaActiva;
} t_worker;

typedef struct {
	t_job* job;
	int master;
	t_worker* nodo;
	int bloque;
	TipoTarea etapa;
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

void iniciarListaEstados();

t_list* TablaEstados;
t_dictionary* diccionarioMasters;
t_yama* leerConfiguracion();
void agregarADiccionarioMaster(int* idUltimoMasterCreado, int socket);

t_log* logYAMA;
#endif /* YAMA_H_ */
