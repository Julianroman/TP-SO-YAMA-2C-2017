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
void inicializarPlanificador();
void finalizar();
t_list* obtenerNodosParaPlanificacion(char* nombreArchivo);
void agregarAListaInfoMaster(payload_RESPUESTA_MASTER* infoMaster);
t_worker* elegirEncargadoReduccionGlobal();
payload_RESPUESTA_MASTER* obtenerSiguienteInfoMaster();
void realizarReduccionGlobal(t_worker* encargado);
t_job *newJob();
void agregarJob(t_job* job);
Tarea getTarea(payload_RESPUESTA_MASTER* infoMaster);
char* getArchivoTemporal(payload_RESPUESTA_MASTER* infoMaster);
void realizarTransformacionNodos();
void realizarReduccionLocal(int id_nodo);
void replanificar(payload_RESPUESTA_MASTER* infoMaster);

// FUNCIONES DE NODO
int getSocketMaster(int id_master);
int registroTerminoExitosamente(t_tablaEstados* registroEstado);
int todosLosNodosTerminaronReduccionLocal();
int nodoTerminoTransformacion(int idJob);
void nodoPasarAReduccionLocal(t_worker* nodo);
t_worker* getNodo(int id);
int estaActivo(t_worker* worker);
t_job* getJobDeNodo(int id);
char* getNombreArchivoTemporalRedLocal(int id_job, int id_nodo);
Tarea etapaActiva(int id_nodo);

// ACTUALIZACIONES
void actualizarEstados(payload_RESPUESTA_MASTER* respuesta);
void actualizarTablaEstados(payload_RESPUESTA_MASTER* respuesta);
void actualizarLog(payload_RESPUESTA_MASTER* infoMaster);
void actualizarEstadosNodo(payload_RESPUESTA_MASTER* respuesta);

//FUNCIONES DE PLANIFICACION
void planificacionWClock(t_list* listaNodos);
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
