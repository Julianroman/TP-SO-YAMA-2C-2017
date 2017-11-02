/*
 * Tarea.c
 *
 *  Created on: 11/10/2017
 *      Author: utnso
 */

#include "Tarea.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

t_tarea *newTarea(){
	t_tarea *tarea = malloc(sizeof(t_tarea));
	tarea->tipo = TAREA_TRANSFORMACION;
	tarea->estado = TAREA_NO_EJECUTADA;

	return tarea;
}

void tareaPasarAReduccionLocal(t_tarea *tarea){
	tarea->tipo = TAREA_REDUCCION_LOCAL;
	tarea->estado = TAREA_NO_EJECUTADA;
}

void tareaPasarAReduccionGlobal(t_tarea *tarea){
	tarea->tipo = TAREA_REDUCCION_GLOBAL;
	tarea->estado = TAREA_NO_EJECUTADA;
}

void tareaPasarAAlmacenamiento(t_tarea *tarea){
	tarea->tipo = TAREA_ALMACENAMIENTO;
	tarea->estado = TAREA_NO_EJECUTADA;
}

int tareaEsTransformacion(t_tarea *tarea){
	return tarea->tipo == TAREA_TRANSFORMACION;
}

int tareaEsReduccionLocal(t_tarea *tarea){
	return tarea->tipo == TAREA_REDUCCION_LOCAL;
}

int tareaEsReduccionGlobal(t_tarea *tarea){
	return tarea->tipo == TAREA_REDUCCION_GLOBAL;
}

int tareaEsAlmacenamiento(t_tarea *tarea){
	return tarea->tipo == TAREA_ALMACENAMIENTO;
}

void tareaMarcarEnEjecucion(t_tarea *tarea, char *nombreResultadoTemporal){
	tarea->estado = TAREA_EN_EJECUCION;
	tarea->nombreResultadoTemporal = strdup(nombreResultadoTemporal);
}

int tareaEstaEnEjecucion(t_tarea *tarea){
	return tarea->estado == TAREA_EN_EJECUCION;
}

int tareaEstaNoEjecutada(t_tarea *tarea){
	return tarea->estado == TAREA_NO_EJECUTADA;
}

int tareaEstaFinalizada(t_tarea *tarea){
	return tarea->estado == TAREA_FINALIZADA;
}

void tareaMarcarNoEJecutada(t_tarea *tarea){
	tarea->estado = TAREA_EN_EJECUCION;
}

void tareaMarcarFinalizada(t_tarea *tarea){
	tarea->estado = TAREA_FINALIZADA;
}

char *tareaObtenerNombreResultadoTemporal(t_tarea *tarea){
	return tarea->nombreResultadoTemporal;
}

void tareaSetearNombreResultadoTemporal(t_tarea* tarea, char* nombreTemporal){
	tarea->nombreResultadoTemporal = nombreTemporal;
}

t_tarea* getTarea(t_worker* worker){
	return worker->tareaActiva;
}
