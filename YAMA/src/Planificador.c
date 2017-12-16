/*
 * Planificador.c
 *
 *  Created on: 20/10/2017
 *      Author: utnso
 */

#include <unistd.h>
#include <commons/string.h>
#include <utilidades/protocol/senders.h>
#include <utilidades/protocol/types.h>
#include "Planificador.h"
#include "YAMA.h"

static int ESTAINICIALIZADO = 0;
static int idUltimoJobCreado = 0;
static int cantidadReplanificaciones = 0;

void iniciarPlanificacion(char* nombreArchivo, t_job_master* job_master){
  usleep(configYAMA->retardoPlanificacion);
  inicializarPlanificador(job_master, nombreArchivo);
  planificacionWClock(job_master);
}

void responderSolicitudMaster(payload_RESPUESTA_MASTER* infoMaster, t_job_master* job_master){

  if(existeEnLista(infoMaster->id_nodo, job_master)){

    actualizarEstados(infoMaster, job_master);

    t_worker* nodo = getNodo(infoMaster->id_nodo);

    if(infoMaster->estado){ //SI LA OPERACION FUE EXITOSA

      switch(etapaActiva(nodo, job_master->job)){
      case TRANSFORMACION:
        if(nodoTerminoTransformacion(infoMaster->id_nodo, job_master->job->id)){
          log_trace(logYAMA, "Nodo %d finalizo transformacion en todos sus bloques.", infoMaster->id_nodo);
          realizarReduccionLocal(nodo, job_master);
        }
        break;
      case REDUCCION_LOCAL:
        if(todosLosNodosTerminaronReduccionLocal(job_master->job->id)){
          job_master->job->etapa = REDUCCION_GLOBAL; //Recien acá sé exactamente que el job está por la etapa de reduccion global
          realizarReduccionGlobal(job_master);
        }
        break;
      case REDUCCION_GLOBAL:
        if(terminoRedGlobal(job_master)){
          job_master->job->etapa = ALMACENAMIENTO;
          realizarAlmacenadoFinal(job_master);
        }
        break;
      case ALMACENAMIENTO:
        log_trace(logYAMA, "JOB %d - NODO %d - ALMACENAMIENTO - EXITO", job_master->job->id, infoMaster->id_nodo);
        finalizarCorrectamente(job_master);
        break;
      }
    }

    else { // SI LA OPERACION FUE ERROR
      if(etapaActiva(nodo, job_master->job) == TRANSFORMACION){
    	if(cantidadReplanificaciones > 0){
    		log_trace(logYAMA, "Replanificando transformacion del Nodo %d ", infoMaster->id_nodo);
    		replanificar(job_master, nodo);
    	}
    	else{
    		log_error(logYAMA, "NO EXISTEN COPIAS DISPONIBLES PARA REPLANIFICAR");
    		abortarJob(job_master);
    	}
      }
      else {
        log_error(logYAMA, "Fallo en etapa NO REPLANIFICABLE");
        abortarJob(job_master);
      }
    }
  }
  else {
    log_warning(logYAMA, "LLEGO INFO DEL NODO %d QUE NO ESTOY TENIENDO EN CUENTA", infoMaster->id_nodo);
  }
}

int existeEnLista(int nodoID, t_job_master* job_master){
  int existe(t_worker* worker){
    return worker->id == nodoID;
  }
  return list_any_satisfy(job_master->nodosActivos, (void*)existe);
}

Tarea etapaActiva(t_worker* nodo, t_job* job){
  t_infoNodo* infoNodo = getInfoNodo(nodo, job);
  return infoNodo->etapaNodo;
}

void abortarJob(t_job_master* job_master){
  job_master->job->estado = ERROR;
  log_error(logYAMA, "JOB %d ABORTADO", job_master->job->id);
  //liberarMemoria(job_master->job);
}

void finalizarCorrectamente(t_job_master* job_master){
  job_master->job->estado = EXITO;
  log_trace(logYAMA, "JOB %d TERMINADO CORRECTAMENTE", job_master->job->id);
  //eliminarMaster(job_master->master_id);
  //liberarMemoria(job_master->job);
}

void liberarMemoria(t_job* job){
  log_trace(logYAMA, "LIBERANDO MEMORIA");
  void liberar(t_tablaEstados* registro){
    free(registro);
  }
  int filtrarPorJob(t_tablaEstados* registro){
    return registro->job->id == job->id;
  }
  // BORRANDO REGISTROS DE LA TABLA DE ESTADOS
  t_list* registrosDeJob = list_filter(TablaEstados, (void*)filtrarPorJob);
  int cantRegistros = list_size(registrosDeJob);
  int i;

  for(i=0; i < cantRegistros; i++){
    list_remove_and_destroy_by_condition(TablaEstados, (void*)filtrarPorJob, (void*)liberar);
  }
  list_destroy(registrosDeJob);

  // BORRANDO INFO DEL PLANIFICADOR
  /*void liberarInfo(t_infoBloque* infoBloque){
    //free(infoBloque); // TODO: Saco esto porque rompe
  }
  int getInfo(t_infoNodo* info){
    return info->job->id = job->id;
  }
  void liberarNodo(t_worker* nodo){
    t_infoNodo* infoNodo = list_find(nodo->infoNodos, (void*)getInfo);
    list_clean_and_destroy_elements(infoNodo->bloquesAEjecutar, (void*)liberarInfo);
    list_clean_and_destroy_elements(infoNodo->infoBloques, (void*)liberarInfo);
    free(infoNodo->job);
    free(infoNodo); // TODO: Saco esto porque rompe
  }
  list_iterate(nodosDisponibles, (void*)liberarNodo);*/
}

void inicializarPlanificador(t_job_master* job_master, char* nombreArchivo){
  if(ESTAINICIALIZADO == 0){
    nodosDisponibles = list_create();
    ESTAINICIALIZADO++;
  }
  job_master->job = newJob();
  job_master->nodosActivos = list_create();
  cargarNodosParaPlanificacion(nombreArchivo, job_master);
}

t_job *newJob(){
  t_job *job = malloc(sizeof(t_job));
  idUltimoJobCreado++;
  job->id = idUltimoJobCreado;
  job->estado = EN_EJECUCION;
  job->etapa = TRANSFORMACION;
  job->cantidadBloquesArchivo = 0;
  return job;
}

t_worker* newNodo(int id, int puerto, char* ip){
  t_worker* nodo = malloc(sizeof(t_worker));
  nodo->id = id;
  nodo->disponibilidad=configYAMA->base;
  nodo->carga=0;
  nodo->puerto = puerto;
  nodo->ip = ip;
  nodo->infoNodos = list_create();
  nodo->cantTareasHistoricas = 0;
  nodo->activo = 1;
  return nodo;
}

void cargarNodosParaPlanificacion(char* nombreArchivo, t_job_master* job_master){
  HEADER_T header;
  //int socketFS = crear_conexion(configYAMA->FS_IP, configYAMA->FS_PUERTO);
  //sem_wait(&binarioSocketFS);
  send_PETICION_NODO(SocketFSGlobal, nombreArchivo);
  void* data = receive(SocketFSGlobal,&header);

  while (header == UBICACION_BLOQUE){

      payload_UBICACION_BLOQUE* bloques = data;
      /*if(bloques->copia == 0){
    	  job_master->job->cantidadBloquesArchivo++;
      }*/
      int nodoConID(t_worker* worker){
        return worker->id == bloques->numero_nodo;
      }

      if(list_any_satisfy(nodosDisponibles, (void*)nodoConID)){ //SI YA LO TENGO AGREGO EL BLOQUE NUEVO
    	t_infoNodo* infoNodo;
        t_worker* nodo = getNodo(bloques->numero_nodo);
        if(!tengoInfoNodo(nodo, job_master->job)){
        	infoNodo = newInfoNodo(nodo, job_master->job);
        	list_add(nodo->infoNodos, infoNodo);
        }
        else{
        	infoNodo = getInfoNodo(nodo, job_master->job);
        }
        t_infoBloque* infoBloque = malloc(sizeof(t_infoBloque));
        infoBloque->bloqueNodo = bloques->bloque_nodo;
        infoBloque->tamanioBloque = bloques->tam_bloque;
        infoBloque->bloqueArchivo = bloques->bloque_archivo;
        infoBloque->copia = bloques->copia;

        list_add(infoNodo->infoBloques, infoBloque);
        log_trace(logYAMA, "Nodo %d bloqueNodo %d, bloqueArchivo %d, copia %d - JOB %d", nodo->id, infoBloque->bloqueNodo, infoBloque->bloqueArchivo, infoBloque->copia, infoNodo->job->id);
      }

      else{ // SI NO LO TENGO EN LA LISTA LO CREO Y LO AGREGO A LA LISTA
        t_worker* nodo = newNodo(bloques->numero_nodo, bloques->puerto, bloques->ip);
        t_infoNodo* nodoInfo = newInfoNodo(nodo, job_master->job);
        list_add(nodo->infoNodos, nodoInfo);

        t_infoBloque* infoBloque = malloc(sizeof(t_infoBloque));
        infoBloque->bloqueNodo = bloques->bloque_nodo;
        infoBloque->tamanioBloque = bloques->tam_bloque;
        infoBloque->bloqueArchivo = bloques->bloque_archivo;
        infoBloque->copia = bloques->copia;

        list_add(nodoInfo->infoBloques, infoBloque);
        list_add(nodosDisponibles, nodo);
        log_trace(logYAMA, "NUEVO NODO %d", nodo->id);
        log_trace(logYAMA, "Nodo %d bloqueNodo %d, bloqueArchivo %d, copia %d - JOB %d", nodo->id, infoBloque->bloqueNodo, infoBloque->bloqueArchivo, infoBloque->copia, nodoInfo->job->id);
      }
    data = receive(SocketFSGlobal,&header);
  }

  if (header == FIN_COMUNICACION){ //Si header es FIN_COMUNICACION es porque se cerro la conexion
    log_error(logYAMA, "Se desconecto el FS.");
    exit(1);
  }
  else if (header == RECHAZO_CONEXION){ //Si header es RECHAZO_CONEXION es porque el fs no esta estable
    log_error(logYAMA, "FS no esta estable, me rechazó.");
    exit(1);
  }
  else if(header == FRACASO_OPERACION){
	  log_error(logYAMA, "No se pudo concretar la operacion");
	  abortarJob(job_master);
  }
  else if(header == FIN_LISTA){
    log_trace(logYAMA, "Se cargaron los nodos correctamente");
  }
  else{
	  log_error(logYAMA, "OPERACION DESCONOCIDA");
  }
}

void realizarTransformacion(t_job_master* job_master){
  int i,j, cantBloques;
  int cantNodos = list_size(job_master->nodosActivos);
  for(i=0; i<cantNodos;i++){
    t_worker* nodo = list_get(job_master->nodosActivos, i);
    t_infoNodo* infoNodo = getInfoNodo(nodo, job_master->job);
    infoNodoPasarAEtapa(infoNodo, TRANSFORMACION);
    cantBloques = list_size(infoNodo->bloquesAEjecutar);
    for(j=0; j<cantBloques; j++){
      t_infoBloque* bloque = list_get(infoNodo->bloquesAEjecutar,j);
      char* nombreArchivoTemporal = getNombreArchivoTemporalTransformacion(job_master->job->id, bloque->bloqueNodo, nodo->id);
      send_INFO_TRANSFORMACION(job_master->master_socket, nodo->puerto, nodo->ip, bloque->bloqueNodo, bloque->tamanioBloque, nombreArchivoTemporal, nodo->id);
      actualizarTablaEstadosConTransformacion(job_master, nodo, bloque->bloqueNodo, nombreArchivoTemporal);
      aumentarCarga(nodo);
    }
  }
  log_trace(logYAMA, "Enviadas todas las transformaciones a los nodos disponibles");
  cantidadReplanificaciones++;
}

void replanificar(t_job_master* job_master, t_worker* nodoFallido){

  t_infoNodo* infoNodo = getInfoNodo(nodoFallido, job_master->job);
  int cantidadBloquesAReplanificar = list_size(infoNodo->bloquesAEjecutar);
  int i, j;

  int sacarNodoCaido(t_worker* worker){
    return worker->id != nodoFallido->id;
  }
  t_list* nodosNoCaidos = list_filter(job_master->nodosActivos, (void*)sacarNodoCaido);

  for(i=0; i < cantidadBloquesAReplanificar; i++){
    t_infoBloque* bloqueABuscar = list_get(infoNodo->bloquesAEjecutar, i);
    int tieneCopia(t_infoBloque* bloqueInfo){
      return bloqueInfo->bloqueArchivo == bloqueABuscar->bloqueArchivo;
    }
    for(j=0; j < list_size(nodosNoCaidos); j++){
      t_worker* worker = list_get(nodosNoCaidos, j);
      t_infoNodo* infoWorker = getInfoNodo(worker, job_master->job);
      if(list_any_satisfy(infoWorker->infoBloques, (void*)tieneCopia)){
        t_infoBloque* bloqueEncontrado = list_find(infoWorker->infoBloques, (void*)tieneCopia);
        char* nombreArchivoTemporal = getNombreArchivoTemporalTransformacion(job_master->job->id, bloqueEncontrado->bloqueNodo, worker->id);
        log_warning(logYAMA, "Bloque %i -- Tamaño %i", bloqueEncontrado->bloqueNodo,  bloqueEncontrado->tamanioBloque);
        send_INFO_TRANSFORMACION(job_master->master_socket, worker->puerto, worker->ip, bloqueEncontrado->bloqueNodo, bloqueEncontrado->tamanioBloque, nombreArchivoTemporal, worker->id);
        actualizarTablaEstadosConTransformacion(job_master, worker, bloqueEncontrado->bloqueNodo, nombreArchivoTemporal);
        break;
      }
    }
  }

  int nodoConID(t_worker* nodo){
    return nodo->id == nodoFallido->id;
  }
  list_remove_by_condition(job_master->nodosActivos, (void*)nodoConID);
  list_destroy(nodosNoCaidos);
}

t_infoBloque* buscarInfoBloque(t_list* bloques, int bloqueArchivo){
  int getBloque(t_infoBloque* infoBloque){
    return infoBloque->bloqueArchivo == bloqueArchivo;
  }
  return list_find(bloques, (void*) getBloque);
}

void infoNodoPasarAEtapa(t_infoNodo* info,  Tarea etapa){
  info->etapaNodo = etapa;
}

void realizarReduccionLocal(t_worker* nodo, t_job_master* job_master){
  int getRegistroEstadoTransformacion(t_tablaEstados* registroEstado){
    return registroEstado->tarea == TRANSFORMACION &&
        registroEstado->nodo->id == nodo->id &&
        registroEstado->estado == EXITO &&
        registroEstado->job->id == job_master->job->id;
  }
  t_list* nodosConTransformacionTerminada = list_filter(TablaEstados, (void*)getRegistroEstadoTransformacion);
  char* nombreTemporalReduccionLocal = getNombreArchivoTemporalRedLocal(job_master->job->id, nodo->id);
  int cantNodos = list_size(nodosConTransformacionTerminada);
  int i;
  for(i=0; i < cantNodos; i++){
    t_tablaEstados* registro = list_get(nodosConTransformacionTerminada, i);
    send_INFO_REDUCCIONLOCAL(job_master->master_socket, nodo->puerto, nodo->ip , registro->archivoTemporal , nombreTemporalReduccionLocal, nodo->id);
  }
  send_FIN_LISTA(job_master->master_socket);
  infoNodoPasarAEtapa(getInfoNodo(nodo, job_master->job), REDUCCION_LOCAL);
  aumentarCarga(nodo);
  actualizarTablaEstadosConReduccion(job_master, nodo, nombreTemporalReduccionLocal, REDUCCION_LOCAL);
  list_destroy(nodosConTransformacionTerminada);
}

void realizarReduccionGlobal(t_job_master* job_master){
  t_worker* encargado = elegirEncargadoRedGlobal(job_master->job);

  int getRegistroEstadoRedLocal(t_tablaEstados* registroEstado){
    return registroEstado->tarea == REDUCCION_LOCAL &&
        registroEstado->estado == EXITO &&
        registroEstado->job->id == job_master->job->id;
  }
  t_list* nodosConRedLocalTerminada = list_filter(TablaEstados, (void*)getRegistroEstadoRedLocal);
  char* nombreTemporalReduccionGlobal = getNombreArchivoTemporalRedGlobal(job_master->job->id, job_master->master_id);
  void realizarRedGlobal(t_tablaEstados* registroEstado){
    send_INFO_REDUCCIONGLOBAL(job_master->master_socket, registroEstado->nodo->puerto, registroEstado->nodo->ip, registroEstado->archivoTemporal, nombreTemporalReduccionGlobal, encargado->id, registroEstado->nodo->id);
    infoNodoPasarAEtapa(getInfoNodo(registroEstado->nodo, job_master->job), REDUCCION_GLOBAL);
  }
  list_iterate(nodosConRedLocalTerminada, (void*)realizarRedGlobal);
  send_FIN_LISTA(job_master->master_socket);
  actualizarTablaEstadosConReduccion(job_master, encargado, nombreTemporalReduccionGlobal, REDUCCION_GLOBAL);
  aumentarCarga(encargado);
  list_destroy(nodosConRedLocalTerminada);
}

t_worker* elegirEncargadoRedGlobal(t_job* job){
  t_list* nodosActivos = getNodosActivos(job->id);
  int ordenar(t_worker* nodo1, t_worker* nodo2){
    if(nodo1->carga == nodo2->carga){
      return nodo1->cantTareasHistoricas < nodo2->cantTareasHistoricas;
    }
    else {
      return nodo1->carga < nodo2->carga;
    }
  }
  list_sort(nodosActivos, (void*)ordenar);
  t_worker* worker = list_get(nodosActivos, 0);
  t_infoNodo* infoNodo = getInfoNodo(worker, job);
  infoNodo->encargado = 1;
  return worker;
}

int terminoRedGlobal(t_job_master* job_master){
  t_worker* encargado = getEncargado(job_master->job);
  int nodoConRedGlobal(t_tablaEstados* registroEstado){
    return registroEstado->tarea == REDUCCION_GLOBAL &&
        registroEstado->job->id == job_master->job->id &&
        registroEstado->nodo->id == encargado->id;
  }
  t_tablaEstados* registro = list_find(TablaEstados, (void*)nodoConRedGlobal);

  if(registro->estado == EXITO) return 1;
  else return 0;
}

void realizarAlmacenadoFinal(t_job_master* job_master){
  int getRegistroEstadoRedLocal(t_tablaEstados* registroEstado){
      return registroEstado->tarea == REDUCCION_GLOBAL &&
          registroEstado->estado == EXITO &&
          registroEstado->job->id == job_master->job->id;
    }
  t_tablaEstados* algunNodoConRedGlobalTerminada = list_find(TablaEstados, (void*)getRegistroEstadoRedLocal);
  t_worker* encargado = getEncargado(job_master->job);
  send_INFO_ALMACENAMIENTO(job_master->master_socket, encargado->puerto, encargado->ip, algunNodoConRedGlobalTerminada->archivoTemporal, encargado->id);
  infoNodoPasarAEtapa(getInfoNodo(encargado, job_master->job), ALMACENAMIENTO);
  log_info(logYAMA, "Job %d - Nodo %d - ALMACENAMIENTO - EN_EJECUCION", job_master->job->id, encargado->id);
}

t_worker* getEncargado(t_job* job){
  int buscarEncargadoDeJob(t_worker* worker){
	  t_infoNodo* infoNodo = getInfoNodo(worker, job);
	  return infoNodo->encargado == 1;
  }
  t_list* nodosActivos = getNodosActivos(job->id);
  t_worker* encargado = list_find(nodosActivos, (void*)buscarEncargadoDeJob);
  return encargado;
}

t_list* getNodosActivos(int jobID){
  int buscarPorJob(t_job_master* job_master){
    return job_master->job->id == jobID;
  }
  t_job_master* mastersJobs = list_find(MastersJobs, (void*)buscarPorJob);
  return mastersJobs->nodosActivos;
}

int registroTerminoExitosamente(t_tablaEstados* registroEstado){
  return registroEstado->estado == EXITO;
}

int todosLosNodosTerminaronReduccionLocal(int jobID){

  int nodoConJOBYReduccionLocal(t_tablaEstados* registroEstado){
    return registroEstado->job->id == jobID &&
        registroEstado->tarea == REDUCCION_LOCAL;
  }

  t_list* nodosEnReduccionLocal = list_filter(TablaEstados, (void*)nodoConJOBYReduccionLocal);

  if(list_size(getNodosActivos(jobID)) == list_size(nodosEnReduccionLocal)){ // La cantidad de nodos en reduccion local tiene que ser igual a la cantidad de nodos activos de ese job
    if(list_all_satisfy(nodosEnReduccionLocal, (void*)registroTerminoExitosamente)){
      list_destroy(nodosEnReduccionLocal);
      return 1;
    }
    else {
      list_destroy(nodosEnReduccionLocal);
      return 0;
    }
  }
  else{
    list_destroy(nodosEnReduccionLocal);
    return 0;
  }
}

int nodoTerminoTransformacion(int idNodo, int jobID){

  int nodoConIDYTransformacion(t_tablaEstados* registroEstado){
    return registroEstado->nodo->id == idNodo &&
        registroEstado->tarea == TRANSFORMACION &&
        registroEstado->job->id == jobID;
  }

  t_list* bloquesEnTransformacion = list_filter(TablaEstados, (void*)nodoConIDYTransformacion);
  if(list_all_satisfy(bloquesEnTransformacion, (void*)registroTerminoExitosamente)){
    list_destroy(bloquesEnTransformacion);
    return 1;
  }
  else {
    list_destroy(bloquesEnTransformacion);
    return 0;
  }
}

char* getNombreArchivoTemporalTransformacion(int jobID, int bloque, int nodoID){
  char* nombre = string_from_format("Job%d-Nodo%d-Bloque%d-TRANSFORMACION",jobID,nodoID,bloque);
  return nombre;
}

char* getNombreArchivoTemporalRedLocal(int jobID, int nodo){
  char* nombre = string_from_format("Job%d-Nodo%d-REDUCCION_LOCAL",jobID,nodo);
  return nombre;
}

char* getNombreArchivoTemporalRedGlobal(int jobID, int masterID){
  char* nombre = string_from_format("Job%d-Master%d-FINAL", jobID, masterID);
  return nombre;
}

t_tablaEstados* getRegistro(payload_RESPUESTA_MASTER* infoMaster, int jobID){
  t_tablaEstados* registro;
  if(infoMaster->bloque == -1){
    int registroEspecifico(t_tablaEstados* registroEstado){
      return registroEstado->nodo->id == infoMaster->id_nodo &&
          registroEstado->estado == EN_EJECUCION &&
          registroEstado->master == infoMaster->id_master &&
          registroEstado->job->id == jobID;
    }
    registro = list_find(TablaEstados, (void*)registroEspecifico);
  }
  else{
    int registroEspecifico(t_tablaEstados* registroEstado){
      return registroEstado->nodo->id == infoMaster->id_nodo &&
          registroEstado->bloque == infoMaster->bloque &&
          registroEstado->estado == EN_EJECUCION &&
          registroEstado->master == infoMaster->id_master &&
          registroEstado->job->id == jobID;
    }
    registro = list_find(TablaEstados, (void*)registroEspecifico);
  }
  return registro;
}

void actualizarEstados(payload_RESPUESTA_MASTER* infoMaster, t_job_master* job_master){
  actualizarTablaEstados(infoMaster, job_master);
  t_worker* nodo = getNodo(infoMaster->id_nodo);
  disminuirCarga(nodo);
  aumentarTareasHistoricas(nodo);
}

void actualizarTablaEstados(payload_RESPUESTA_MASTER* infoMaster, t_job_master* job_master){
  if(infoMaster->bloque != -2){ // Si la respuesta es distinta a almacenamiento
    t_tablaEstados* registroEstado = getRegistro(infoMaster, job_master->job->id);
    if(registroEstado->tarea == TRANSFORMACION){
      if(infoMaster->estado){
        registroEstado->estado = EXITO;
        log_info(logYAMA, "Job %d - Nodo %d - Bloque Archivo %d - %s - EXITO", registroEstado->job->id, infoMaster->id_nodo, infoMaster->bloque, castearTarea(registroEstado->tarea));
      }
      else {
        registroEstado->estado = ERROR;
        log_error(logYAMA, "Job %d - Nodo %d - Bloque Archivo %d - %s - ERROR", registroEstado->job->id, infoMaster->id_nodo, infoMaster->bloque, castearTarea(registroEstado->tarea));
      }
    }
    else{
      if(infoMaster->estado){
        registroEstado->estado = EXITO;
        log_info(logYAMA, "Job %d - Nodo %d - %s - EXITO", registroEstado->job->id, infoMaster->id_nodo, castearTarea(registroEstado->tarea));
      }
      else {
        registroEstado->estado = ERROR;
        log_error(logYAMA, "Job %d - Nodo %d - %s - ERROR", registroEstado->job->id, infoMaster->id_nodo, castearTarea(registroEstado->tarea));
      }
    }
  }
}

void actualizarTablaEstadosConTransformacion(t_job_master* job_master, t_worker* nodo, int bloque, char* nombreArchivoTemporal){
  t_tablaEstados* registroEstado = malloc(sizeof(t_tablaEstados));
  registroEstado->job = job_master->job;
  registroEstado->master = job_master->master_id;
  registroEstado->nodo = nodo;
  registroEstado->bloque = bloque;
  registroEstado->tarea = TRANSFORMACION;
  registroEstado->archivoTemporal = nombreArchivoTemporal;
  registroEstado->estado = EN_EJECUCION;

  list_add(TablaEstados, registroEstado);
  char* tarea = castearTarea(registroEstado->tarea);
  char* estado = castearEstado(registroEstado->estado);
  log_info(logYAMA, "Job %d - Nodo %d - Bloque Nodo %d - %s - %s", registroEstado->job->id, registroEstado->nodo->id, registroEstado->bloque, tarea, estado);
}

void actualizarTablaEstadosConReduccion(t_job_master* job_master, t_worker* nodo, char* nombreArchivoTemporal, Tarea etapa){
  t_tablaEstados* registroEstado = malloc(sizeof(t_tablaEstados));
  registroEstado->job = job_master->job;
  registroEstado->master = job_master->master_id;
  registroEstado->nodo = nodo;
  registroEstado->tarea = etapa;
  registroEstado->archivoTemporal = nombreArchivoTemporal;
  registroEstado->estado = EN_EJECUCION;

  list_add(TablaEstados, registroEstado);
  log_info(logYAMA, "Job %d - Nodo %d - %s - %s", job_master->job->id, nodo->id, castearTarea(etapa), castearEstado(EN_EJECUCION));
}

t_worker* getNodo(int nodoID){
  int nodoConId(t_worker* nodo){
    return nodo->id == nodoID;
  }
  t_worker* nodo =  list_find(nodosDisponibles, (void*)nodoConId);
  return nodo;
}

int tengoInfoNodo(t_worker* nodo, t_job* job){
	int buscarInfo(t_infoNodo* infoNodo){
	    return infoNodo->job->id == job->id;
	  }
	return list_any_satisfy(nodo->infoNodos, (void*)buscarInfo);
}

t_infoNodo* newInfoNodo(t_worker* nodo, t_job* job){
		t_infoNodo* info = malloc(sizeof(t_infoNodo));
	  info->job = job;
	  info->infoBloques = list_create();
	  info->bloquesAEjecutar = list_create();
	  info->etapaNodo = TRANSFORMACION;
	  info->encargado = 0;
	  return info;
}

t_infoNodo* getInfoNodo(t_worker* nodo, t_job* job){
  int buscarInfo(t_infoNodo* infoNodo){
    return infoNodo->job->id == job->id;
  }
  t_infoNodo* info = list_find(nodo->infoNodos, (void*)buscarInfo);
  return info;
}

char* getArchivoTemporal(payload_RESPUESTA_MASTER* infoMaster){
  int registroEspecifico(t_tablaEstados* registroEstado){
        return registroEstado->nodo->id == infoMaster->id_nodo &&
            registroEstado->bloque == infoMaster->bloque	&&
            registroEstado->estado == infoMaster->estado &&
            registroEstado->master == infoMaster->id_master;
      }
  t_tablaEstados* registroEstado = list_find(TablaEstados, (void*)registroEspecifico);
  return registroEstado->archivoTemporal;
}

int getTotalBloquesArchivo(int jobID){
  int max=0;

  void maximo(t_infoBloque* bloque){
      if(bloque->bloqueArchivo > max){
        max = bloque->bloqueArchivo;
      }
    }
  void iterarInfo(t_infoNodo* nodoInfo){
    if(nodoInfo->job->id == jobID){
      list_iterate(nodoInfo->infoBloques, (void*)maximo);
    }
  }
  void sumarTotal(t_worker* worker){
    list_iterate(worker->infoNodos, (void*)iterarInfo);
  }

  list_iterate(nodosDisponibles, (void*)sumarTotal);
  return max+1;
}

void planificacionWClock(t_job_master* job_master){
  int contador = 0;
  int cantNodos = list_size(nodosDisponibles);

  list_iterate(nodosDisponibles, (void*)calcularDisponibilidad);

  ordenarListaNodosPorDisponibilidad(nodosDisponibles);

  void pasarASiguiente(){
    if(contador < cantNodos-1){
      contador += 1;
    }
    else{
      contador = 0;
    }
  }

  int bloqueArchivo;
  int cantidadTotalBloquesArchivo = getTotalBloquesArchivo(job_master->job->id);
  //int cantidadTotalBloquesArchivo = 212;
  //int cantidadTotalBloquesArchivo = job_master->job->cantidadBloquesArchivo;
  for(bloqueArchivo = 0; bloqueArchivo < cantidadTotalBloquesArchivo; bloqueArchivo++){
    while(1){
      t_worker* workerActual = list_get(nodosDisponibles, contador);
      t_infoNodo* infoNodo = getInfoNodo(workerActual, job_master->job);

      if(existeEn(infoNodo->infoBloques, bloqueArchivo) != 0){
        if(workerActual->disponibilidad > 0){
          t_infoBloque* infoBloque = buscarInfoBloque(infoNodo->infoBloques, bloqueArchivo);
          list_add(infoNodo->bloquesAEjecutar, infoBloque);
          aumentarCarga(workerActual);
          disminuirDisponibilidad(workerActual);
          log_trace(logYAMA, "ASIGNADO NODO %d BLOQUE ARCHIVO %i",workerActual->id, bloqueArchivo);
          int estaEnLista(t_worker* nodito){
            return nodito->id == workerActual->id;
          }
          if(!list_any_satisfy(job_master->nodosActivos, (void*)estaEnLista)){
            list_add(job_master->nodosActivos, workerActual);
          }
          pasarASiguiente();
          break;
        }
        else{
          workerActual->disponibilidad = configYAMA->base;
        }
      }
      pasarASiguiente();
    }
  }
  log_trace(logYAMA, "Planificacion terminada. Mandando a realizar instrucciones a los nodos");
  realizarTransformacion(job_master);

}

int existeEn(t_list* listaBloques , int bloqueArchivo){
  int existeBloque(t_infoBloque* infoBloque){
    return infoBloque->bloqueArchivo == bloqueArchivo;
  }
  return list_any_satisfy(listaBloques, (void*) existeBloque);
}

void ordenarListaNodosPorDisponibilidad(t_list* listaNodos){ // ordena la lista de nodos segun la disponibilidad
  int mayorDisponibilidad(t_worker* nodo1, t_worker* nodo2){
    if(disponibilidad(nodo1) == disponibilidad(nodo2)){
      return tareasHistoricas(nodo1) < tareasHistoricas(nodo2);
    }
    else
      return disponibilidad(nodo1) > disponibilidad(nodo2);
    }
  list_sort(listaNodos,(void*)mayorDisponibilidad);
}

int PWL(t_worker* worker){
  return WLmax() + carga(worker);
}

int WLmax(){ // ordena la lista de nodos segun la disponibilidad
  int mayorCarga(t_worker* nodo1, t_worker* nodo2){
      return carga(nodo1) > carga(nodo2);
    }
  list_sort(nodosDisponibles,(void*)mayorCarga);
  t_worker* worker = list_get(nodosDisponibles, 0);
  return carga(worker);
}

int carga(t_worker* worker){
  return worker->carga;
}
void aumentarCarga(t_worker* nodo){
  nodo->carga += 1;
}
void disminuirCarga(t_worker* nodo){
  nodo->carga -= 1;
}
void calcularDisponibilidad(t_worker* worker){
  if(string_equals_ignore_case(configYAMA->algoritmoBalanceo,"WCLOCK")){
    worker->disponibilidad += PWL(worker);
  }
}
int disponibilidad(t_worker* worker){
  return worker->disponibilidad;
}
void disminuirDisponibilidad(t_worker* worker){
  worker->disponibilidad -= 1;
}
int tareasHistoricas(t_worker* worker){
  return worker->cantTareasHistoricas;
}
void aumentarTareasHistoricas(t_worker* worker){
  worker->cantTareasHistoricas += 1;
}
int estaActivo(t_worker* worker){
  return worker->activo == 1;
}
char* castearTarea(Tarea tarea){
  switch(tarea){
  case TRANSFORMACION:
    return "TRANSFORMACION";
  case REDUCCION_LOCAL:
    return "REDUCCION_LOCAL";
  case REDUCCION_GLOBAL:
    return "REDUCCION_GLOBAL";
  case ALMACENAMIENTO:
    return "ALMACENAMIENTO";
  }
}
char* castearEstado(YAMA_STATUS estado){
  switch(estado){
  case EN_EJECUCION:
    return "EN_EJECUCION";
  case EXITO:
    return "EXITO";
  case ERROR:
    return "ERROR";
  }
}
