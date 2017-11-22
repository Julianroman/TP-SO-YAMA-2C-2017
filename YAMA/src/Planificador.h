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
#include <utilidades/protocol/senders.h>

t_list* nodosDisponibles;
t_dictionary* bloques_ejecutados;
t_dictionary* diccionarioJobs;
t_dictionary* diccionarioNodos;
t_list* listaRespuestasMaster;

// FUNCIONES DE PLANIFICADOR
void iniciarPlanificacion(char* nombreArchivo);
int inicializarPlanificador();
void finalizarCorrectamente(int jobID);
void abortarJob(int jobID);
t_list* cargarNodosParaPlanificacion(char* nombreArchivo);
t_worker* elegirEncargadoReduccionGlobal(int jobID);
void realizarReduccionGlobal(t_worker* encargado);
void realizarTransformacionNodos(int jobID);
void realizarReduccionLocal(t_worker* nodo, int jobID);
void replanificar(payload_RESPUESTA_MASTER* infoMaster, int jobID);
//FUNCIONES DE JOB
t_job *newJob();
t_job* getJob(int jobID);
int agregarJob(t_job* job);

// UTILES
t_list* getNodosDeJob(int jobID);
void agregarListaNodos(t_list* listaNodos, int jobID);
Tarea getTarea(payload_RESPUESTA_MASTER* infoMaster);
Tarea etapaActiva(t_worker* nodo);
char* getArchivoTemporal(payload_RESPUESTA_MASTER* infoMaster);
void agregarAListaInfoMaster(payload_RESPUESTA_MASTER* infoMaster);
payload_RESPUESTA_MASTER* obtenerSiguienteInfoMaster();
char* getNombreArchivoTemporalRedLocal(int jobID, int nodoID);
int getSocketMaster(int id_master);
int registroTerminoExitosamente(t_tablaEstados* registroEstado);

// FUNCIONES DE NODO
int todosLosNodosTerminaronReduccionLocal(int jobID);
int nodoTerminoTransformacion(int idJob);
void nodoPasarAReduccionLocal(t_worker* nodo);
t_worker* getNodo(int nodoID, int jobID);
int estaActivo(t_worker* worker);
t_job* getJobDeNodo(int nodoID);

// ACTUALIZACIONES
void actualizarEstados(payload_RESPUESTA_MASTER* respuesta);
void actualizarTablaEstados(payload_RESPUESTA_MASTER* respuesta);
void actualizarLog(payload_RESPUESTA_MASTER* infoMaster);

//FUNCIONES DE PLANIFICACION
void planificacionWClock(int jobID);
int existeEn(t_list* lista , char* dato);
int obtenerDisponibilidadNodo(t_worker* worker);
int PWL(t_worker* worker);
int WLmax();
int carga(t_worker* worker);
void nodoConMayorDisponibilidad(int jobID);
void calcularDisponibilidad(t_worker* worker);
int disponibilidad(t_worker* worker);
int tareasHistoricas(t_worker* worker);

#endif /* PLANIFICADOR_H_ */
