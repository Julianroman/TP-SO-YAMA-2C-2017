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

t_list* listaNodos;
t_dictionary* bloques_ejecutados;
t_dictionary* diccionarioJobs;
t_list* listaRespuestasMaster;

// FUNCIONES DE PLANIFICADOR
void iniciarPlanificacion(char* nombreArchivo);
int inicializarPlanificador();
void finalizarCorrectamente(int jobID);
t_list* obtenerNodosParaPlanificacion(char* nombreArchivo);
void agregarAListaInfoMaster(payload_RESPUESTA_MASTER* infoMaster);
t_worker* elegirEncargadoReduccionGlobal();
payload_RESPUESTA_MASTER* obtenerSiguienteInfoMaster();
void realizarReduccionGlobal(t_worker* encargado);
Tarea getTarea(payload_RESPUESTA_MASTER* infoMaster);
char* getArchivoTemporal(payload_RESPUESTA_MASTER* infoMaster);
void realizarTransformacionNodos(int jobID);
void realizarReduccionLocal(int nodoID, int jobID);
void replanificar(payload_RESPUESTA_MASTER* infoMaster);

//FUNCIONES DE JOB
t_job *newJob();
t_job* getJob(int jobID);
int agregarJob(t_job* job);

// FUNCIONES DE NODO
int getSocketMaster(int id_master);
int registroTerminoExitosamente(t_tablaEstados* registroEstado);
int todosLosNodosTerminaronReduccionLocal(int idnodoID_nodo, int jobID);
int nodoTerminoTransformacion(int idJob);
void nodoPasarAReduccionLocal(t_worker* nodo);
t_worker* getNodo(int nodoID);
int estaActivo(t_worker* worker);
t_job* getJobDeNodo(int nodoID);
char* getNombreArchivoTemporalRedLocal(int jobID, int nodoID);
Tarea etapaActiva(int nodoID);

// ACTUALIZACIONES
void actualizarEstados(payload_RESPUESTA_MASTER* respuesta);
void actualizarTablaEstados(payload_RESPUESTA_MASTER* respuesta);
void actualizarLog(payload_RESPUESTA_MASTER* infoMaster);
void actualizarEstadosNodo(payload_RESPUESTA_MASTER* respuesta);

//FUNCIONES DE PLANIFICACION
void planificacionWClock(t_list* listaNodos, int jobID);
int existeEn(t_list* lista , char* dato);
int obtenerDisponibilidadNodo(t_worker* worker);
int PWL(t_worker* worker);
int WLmax();
int carga(t_worker* worker);
void nodoConMayorDisponibilidad();
void calcularDisponibilidad(t_worker* worker);
int disponibilidad(t_worker* worker);
int tareasHistoricas(t_worker* worker);

#endif /* PLANIFICADOR_H_ */
