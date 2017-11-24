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

t_list* nodosDisponibles; //Lista de nodos - cada una pertenece a un JOB
t_dictionary* diccionarioJobNodos; //JOB ID - Lista nodos para ese JOB

// FUNCIONES DE PLANIFICADOR
void iniciarPlanificacion(char* nombreArchivo, t_job_master* job_master);
void responderSolicitudMaster(payload_RESPUESTA_MASTER* infoMaster, t_job_master* job_master);
void inicializarPlanificador();
void finalizarCorrectamente(t_job* job);
void abortarJob(t_job* job);
t_list* cargarNodosParaPlanificacion(char* nombreArchivo);
t_worker* elegirEncargadoReduccionGlobal(int jobID);
void realizarReduccionGlobal(t_worker* encargado);
void realizarTransformacionNodos(t_job_master* job_master);
void realizarReduccionLocal(t_worker* nodo, t_job_master* job_master);
void replanificar(payload_RESPUESTA_MASTER* infoMaster, int jobID);

//FUNCIONES DE JOB
t_job *newJob();

// UTILES
t_list* getNodosDeJob(int jobID);
void agregarListaNodosAJob(t_list* listaNodos, int jobID);
Tarea getTarea(payload_RESPUESTA_MASTER* infoMaster);
Tarea etapaActiva(t_worker* nodo);
char* getArchivoTemporal(payload_RESPUESTA_MASTER* infoMaster);
char* getNombreArchivoTemporalRedLocal(int jobID, int nodoID);
int registroTerminoExitosamente(t_tablaEstados* registroEstado);

// FUNCIONES DE NODO
int todosLosNodosTerminaronReduccionLocal(int jobID);
int nodoTerminoTransformacion(int idJob);
void nodoPasarAReduccionLocal(t_worker* nodo);
void nodoPasarATransformacion(t_worker* nodo);
t_worker* getNodo(int nodoID, int jobID);
int estaActivo(t_worker* worker);

// ACTUALIZACIONES
void actualizarEstados(payload_RESPUESTA_MASTER* respuesta, t_job_master* job_master);
void actualizarTablaEstados(payload_RESPUESTA_MASTER* respuesta, t_job_master* job_master);
t_tablaEstados* getRegistro(payload_RESPUESTA_MASTER* infoMaster, int jobID);
void agregarRegistroATablaEstados(payload_RESPUESTA_MASTER* infoMaster, t_job_master* job_master);
void actualizarLog(payload_RESPUESTA_MASTER* infoMaster);
void actualizarTablaEstadosConTransformacion(t_job_master* job_master, t_worker* nodo, int bloque, char* nombreArchivoTemporal);

//FUNCIONES DE PLANIFICACION
void planificacionWClock(t_job_master* job_master);
int existeEn(t_list* lista , char* dato);
int obtenerDisponibilidadNodo(t_worker* worker);
int PWL(t_worker* worker);
int WLmax();
int carga(t_worker* worker);
void nodoConMayorDisponibilidad(t_list* listaNodos);
void calcularDisponibilidad(t_worker* worker);
int disponibilidad(t_worker* worker);
int tareasHistoricas(t_worker* worker);

#endif /* PLANIFICADOR_H_ */
