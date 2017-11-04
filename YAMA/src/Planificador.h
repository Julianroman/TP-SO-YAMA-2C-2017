/*
 * Planificador.h
 *
 *  Created on: 20/10/2017
 *      Author: utnso
 */

#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_

#include "YAMA.h"
#include <utilidades/protocol/types.h>

t_list* listaNodos;
t_dictionary* bloques_ejecutados;
t_dictionary* diccionarioJobs;
t_dictionary* diccionarioTareas;
t_list* listaRespuestasMaster;

int idUltimoJobCreado;

void iniciarPlanificacion(char* nombreArchivo);
void inicializarPlanificador();
t_list* obtenerNodosParaPlanificacion(char* nombreArchivo);
void agregarJob(t_job* job);
void iniciarPlanificacion();
void agregarAListaInfoMaster(payload_RESPUESTA_MASTER* infoMaster);
t_worker* elegirEncargadoReduccionGlobal();
void realizarSiguienteTarea(int idWorker);
int todosLosNodosTerminaronReduccionLocal(t_list* nodosDisponibles);
int todosLosNodosTerminaronTransformacion(t_list* nodosDisponibles);
void realizarSiguienteinstruccion(payload_RESPUESTA_MASTER* respuesta);
payload_RESPUESTA_MASTER* obtenerSiguienteInfoMaster();
void realizarReduccionGlobal(t_worker* encargado);
void actualizarEstados(payload_RESPUESTA_MASTER* respuesta);
void actualizarTablaEstados(payload_RESPUESTA_MASTER* respuesta);
void actualizarEstadosNodo(payload_RESPUESTA_MASTER* respuesta);
void planificacionWClock(t_list* listaNodos);
int existeEn(t_list* lista , char* dato);
int obtenerDisponibilidadNodo(t_worker* worker);
t_worker* buscarNodo(t_list* listaNodos, int numNodo);
int estaActivo(t_worker* worker);
int PWL(t_worker* worker);
int WLmax();
int carga(t_worker* worker);
void calcularDisponibilidad(t_worker* worker);
int disponibilidad(t_worker* worker);
int tareasHistoricas(t_worker* worker);
t_tarea* getTarea(t_worker* worker);
t_job* getJobDeWorker(int id);
t_job *newJob();
#endif /* PLANIFICADOR_H_ */
