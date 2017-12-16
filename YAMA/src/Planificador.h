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
#include <utilidades/socket_utils.h>
#include "serverYAMA/serverYAMA.h"

extern t_yama* configYAMA;
extern sem_t binarioSocketFS;
extern int SocketFSGlobal;
t_list* nodosDisponibles;

// FUNCIONES DE PLANIFICADOR
void iniciarPlanificacion(char* nombreArchivo, t_job_master* job_master);
void responderSolicitudMaster(payload_RESPUESTA_MASTER* infoMaster, t_job_master* job_master);
void inicializarPlanificador(t_job_master* job_master, char* nombreArchivo);
void cargarNodosParaPlanificacion(char* nombreArchivo, t_job_master* job_master);
t_worker* elegirEncargadoRedGlobal(t_job* job);
void realizarAlmacenadoFinal(t_job_master* job_master);
void realizarReduccionGlobal(t_job_master* job_master);
void realizarTransformacion(t_job_master* job_master);
void realizarReduccionLocal(t_worker* nodo, t_job_master* job_master);
void replanificar(t_job_master* job_master, t_worker* nodoFallido);
t_worker* getEncargado(t_job* job);
int terminoRedGlobal(t_job_master* job_master);

//FUNCIONES DE JOB
t_job *newJob();
void abortarJob(t_job_master* job_master);
void finalizarCorrectamente(t_job_master* job_master);

// UTILES
char* castearTarea(Tarea tarea);
char* castearEstado(YAMA_STATUS estado);
char* getArchivoTemporal(payload_RESPUESTA_MASTER* infoMaster);
char* getNombreArchivoTemporalRedLocal(int jobID, int nodoID);
char* getNombreArchivoTemporalTransformacion(int jobID, int bloque, int nodoID);
char* getNombreArchivoTemporalRedGlobal(int jobID, int masterID);
int registroTerminoExitosamente(t_tablaEstados* registroEstado);
t_infoBloque* buscarInfoBloque(t_list* bloques, int bloqueArchivo);
void liberarMemoria(t_job* job);
int existeEnLista(int nodoID, t_job_master* job_master);
t_list* getNodosActivos(int jobID);

// FUNCIONES DE NODO
t_infoNodo* getInfoNodo(t_worker* nodo, t_job* job);
int todosLosNodosTerminaronReduccionLocal(int jobID);
int nodoTerminoTransformacion(int nodoID, int jobID);
void infoNodoPasarAEtapa(t_infoNodo* info,  Tarea etapa);
Tarea etapaActiva(t_worker* nodo, t_job* job);
t_worker* getNodo(int nodoID);
int estaActivo(t_worker* worker);
void aumentarCarga(t_worker* nodo);
void disminuirCarga(t_worker* nodo);
t_infoNodo* newInfoNodo(t_worker* nodo, t_job* job);
int tengoInfoNodo(t_worker* nodo, t_job* job);

// ACTUALIZACIONES
void actualizarEstados(payload_RESPUESTA_MASTER* respuesta, t_job_master* job_master);
void actualizarTablaEstados(payload_RESPUESTA_MASTER* respuesta, t_job_master* job_master);
t_tablaEstados* getRegistro(payload_RESPUESTA_MASTER* infoMaster, int jobID);
void actualizarTablaEstadosConTransformacion(t_job_master* job_master, t_worker* nodo, int bloque, char* nombreArchivoTemporal);
void actualizarTablaEstadosConReduccion(t_job_master* job_master, t_worker* nodo, char* nombreArchivoTemporal, Tarea etapa);

//FUNCIONES DE PLANIFICACION
void planificacionWClock(t_job_master* job_master);
int existeEn(t_list* lista , int bloqueArchivo);
int obtenerDisponibilidadNodo(t_worker* worker);
int PWL(t_worker* worker);
int WLmax();
int carga(t_worker* worker);
void aumentarCarga(t_worker* nodo);
void disminuirCarga(t_worker* nodo);
void ordenarListaNodosPorDisponibilidad(t_list* listaNodos);
void calcularDisponibilidad(t_worker* worker);
int disponibilidad(t_worker* worker);
void disminuirDisponibilidad(t_worker* worker);
int tareasHistoricas(t_worker* worker);
void aumentarTareasHistoricas(t_worker* worker);

#endif /* PLANIFICADOR_H_ */
