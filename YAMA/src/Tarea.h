/*
 * Tarea.h
 *
 *  Created on: 11/10/2017
 *      Author: utnso
 */

#ifndef TAREA_H_
#define TAREA_H_


typedef enum {TAREA_TRANSFORMACION, TAREA_REDUCCION_LOCAL, TAREA_REDUCCION_GLOBAL, TAREA_ALMACENAMIENTO} TipoTarea;

typedef enum {TAREA_NO_EJECUTADA, TAREA_EN_EJECUCION, TAREA_FINALIZADA} EstadoTarea;


typedef struct {
	uint16_t id;
	char *nombreResultadoTemporal;
	EstadoTarea estado;
	TipoTarea tipo;
} t_tarea;

t_tarea *newTarea();

void tareaPasarAReduccionLocal(t_tarea *tarea);

void tareaPasarAReduccionGlobal(t_tarea *tarea);

void tareaPasarAAlmacenamiento(t_tarea *tarea);

char *tareaObtenerNombreResultadoTemporal(t_tarea *tarea);

int tareaEstaNoEjecutada(t_tarea *tarea);

int tareaEstaEnEjecucion(t_tarea *tarea);

int tareaEstaFinalizada(t_tarea *tarea);

void tareaMarcarEnEjecucion(t_tarea *tarea, char *nombreResultadoTemporal);

void tareaMarcarNoEjecutada(t_tarea *tarea);

void tareaMarcarFinalizada(t_tarea *tarea);

int tareaEsTransformacion(t_tarea *tarea);

int tareaEsReduccionGlobal(t_tarea *tarea);

int tareaEsReduccionLocal(t_tarea *tarea);

#endif /* TAREA_H_ */
