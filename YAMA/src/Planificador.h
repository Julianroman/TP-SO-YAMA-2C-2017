/*
 * Planificador.h
 *
 *  Created on: 20/10/2017
 *      Author: utnso
 */

#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_

#include "YAMA.h"

t_list* jobsAPlanificar;

typedef struct {

	int numero;
	char *ip;
	char *puerto;
	int carga;
	t_list* bloques;
	int disponibilidad;
	int activo;
	int cantTareasHistoricas;
	t_job* jobActivo;

} t_nodo;

t_list* listaNodos;
t_dictionary* bloques_ejecutados;
t_dictionary* diccionarioJobs;
t_list* listaRespuestasMaster;

int idUltimoJobCreado;
int idUltimoJobPlanificado;
int idUltimaTarea;

void inicializarPlanificador();
void agregarJob(t_job* job);
void iniciarPlanificacion();
int todosLosNodosTerminaronReduccionLocal(t_list* nodosDisponibles);
void realizarSiguienteinstruccion(respuestaInfoMaster* respuesta);
respuestaInfoMaster* obtenerSiguienteInfoMaster();
void actualizarEstados(respuestaInfoMaster* respuesta);
void actualizarTablaEstados(respuestaInfoMaster* respuesta);
void actualizarEstadosNodo(respuestaInfoMaster* respuesta);
void planificacionClock(t_list* listaNodos);
int existeEn(t_list* lista , void* dato);
int obtenerDisponibilidadNodo(t_nodo* worker);
t_nodo* buscarNodo(t_list* listaNodos, int numNodo);
int estaActivo(t_nodo* worker);

#endif /* PLANIFICADOR_H_ */
