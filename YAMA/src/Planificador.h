/*
 * Planificador.h
 *
 *  Created on: 20/10/2017
 *      Author: utnso
 */

#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_

#include "YAMA.h"

t_list* listaNodos;
t_dictionary* bloques_ejecutados;
t_dictionary* diccionarioJobs;
t_list* listaRespuestasMaster;
t_list* tablaEstados;

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
int obtenerDisponibilidadNodo(t_worker* worker);
t_worker* buscarNodo(t_list* listaNodos, int numNodo);
int estaActivo(t_worker* worker);

#endif /* PLANIFICADOR_H_ */
