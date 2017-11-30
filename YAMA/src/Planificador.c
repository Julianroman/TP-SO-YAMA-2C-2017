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
int base = 2;


void iniciarPlanificacion(char* nombreArchivo, t_job_master* job_master){
	usleep(configYAMA->retardoPlanificacion);
	inicializarPlanificador(job_master, nombreArchivo);
	planificacionWClock(job_master);
}

void responderSolicitudMaster(payload_RESPUESTA_MASTER* infoMaster, t_job_master* job_master){

	actualizarEstados(infoMaster, job_master);

	t_worker* nodo = getNodo(infoMaster->id_nodo, job_master->job->id);

	if(infoMaster->estado){ //SI LA OPERACION FUE EXITOSA

		switch(etapaActiva(nodo)){
		case TRANSFORMACION:
			if(nodoTerminoTransformacion(infoMaster->id_nodo, job_master->job->id)){
				log_trace(logYAMA, "El nodo %d finalizo transformacion en todos sus bloques. Pasando a reduccion local", infoMaster->id_nodo);
				realizarReduccionLocal(nodo, job_master);
			}
			else{
				log_trace(logYAMA, "El nodo %d finalizo transformacion en el bloque %d", infoMaster->id_nodo, infoMaster->bloque);
			}
			break;
		case REDUCCION_LOCAL:
			log_trace(logYAMA, "El nodo %d finalizo Reduccion Local", infoMaster->id_nodo);
			if(todosLosNodosTerminaronReduccionLocal(job_master->job->id)){
				realizarReduccionGlobal(job_master);
			}
			break;
		case REDUCCION_GLOBAL:
			//QUE SE HACE?
			// Realizar almacenamiento final
			break;
		case ALMACENAMIENTO:
			// QUE SE HACE?
			finalizarCorrectamente(job_master->job);
			break;
		}
	}

	else { // SI LA OPERACION FUE ERROR
		if(etapaActiva(nodo) == TRANSFORMACION){
			log_trace(logYAMA, "Replanificando transformacion del Nodo %d ", infoMaster->id_nodo);
			replanificar(job_master, nodo);
		}
		else {
			log_error(logYAMA, "Fallo en etapa NO REPLANIFICABLE - Abortando JOB");
			abortarJob(job_master->job);
		}
	}
}

void abortarJob(t_job* job){
	job->estado = ERROR;
	list_destroy(getNodosDeJob(job->id));
	log_trace(logYAMA, "JOB TERMINADO ERRONEAMENTE");
}

void finalizarCorrectamente(t_job* job){
	job->estado = EXITO;
	list_destroy(getNodosDeJob(job->id));
	log_trace(logYAMA, "JOB TERMINADO CORRECTAMENTE");
}

void inicializarPlanificador(t_job_master* job_master, char* nombreArchivo){
	if(ESTAINICIALIZADO == 0){
		diccionarioJobNodos = dictionary_create();
		ESTAINICIALIZADO++;
	}
	job_master->job = newJob();
	t_list* nodosDisponibles = cargarNodosParaPlanificacion(nombreArchivo, job_master->job->id);
	agregarListaNodosAJob(nodosDisponibles, job_master->job->id);
}

t_job *newJob(){
	t_job *job = malloc(sizeof(t_job));
	idUltimoJobCreado++;
	job->id = idUltimoJobCreado;
	job->estado = EN_EJECUCION;
	job->etapa = TRANSFORMACION;
	return job;
}

t_worker* newNodo(int id, int puerto, char* ip){
	t_worker* nodo = malloc(sizeof(t_worker));
	nodo->id = id;
	nodo->disponibilidad=configYAMA->base;
	nodo->carga=0;
	nodo->puerto = puerto;
	nodo->ip = ip;
	nodo->infoBloques = list_create();
	nodo->cantTareasHistoricas = 0;
	nodo->activo = 1;
	nodo->bloquesAEjecutar = list_create();
	return nodo;
}

void agregarListaNodosAJob(t_list* listaNodos, int jobID){
	char* keyJob = string_itoa(jobID);
	dictionary_put(diccionarioJobNodos, keyJob, listaNodos);
}

t_list* getNodosDeJob(int jobID){
	char* keyJob = string_itoa(jobID);
	return dictionary_get(diccionarioJobNodos, keyJob);
}

t_list* cargarNodosParaPlanificacion(char* nombreArchivo, int jobID){
	HEADER_T header;
	int socketFS = crear_conexion(configYAMA->FS_IP, configYAMA->FS_PUERTO);
	send_PETICION_NODO(socketFS, nombreArchivo);

	t_list* nodosDisponibles = list_create();

	/*if (header == FIN_COMUNICACION){ //Si header es FIN_COMUNICACION es porque se cerro la conexion
		//FD_CLR(socketFS,&master); // Eliminar de la lista
		break;
	}*/

	void* data = receive(socketFS,&header);
	payload_UBICACION_BLOQUE* bloques = data;

	int nodoConID(t_worker* worker){
		return worker->id == bloques->numero_nodo;
	}

	while (header != FIN_LISTA){

		if(list_any_satisfy(nodosDisponibles, (void*)nodoConID)){ //SI YA LO TENGO AGREGO LO NUEVO
			t_worker* nodo = getNodo(bloques->numero_nodo, jobID);

			t_infoBloque* infoBloque = malloc(sizeof(t_infoBloque));
			infoBloque->bloqueNodo = bloques->bloque_nodo;
			infoBloque->bloqueArchivo = bloques->bloque_archivo;
			infoBloque->copia = bloques->copia;

			list_add(nodo->infoBloques, infoBloque);
			log_trace(logYAMA, "Se agregó al nodo %d con bloqueNodo %d, bloqueArchivo %d y copia %d", nodo->id, infoBloque->bloqueNodo, infoBloque->bloqueArchivo, infoBloque->copia);
		}

		else{ // SI NO LO TENGO EN LA LISTA LO CREO Y LO AGREGO A LA LISTA
			t_worker* nodo = newNodo(bloques->numero_nodo, bloques->ip, bloques->puerto);

			t_infoBloque* infoBloque = malloc(sizeof(t_infoBloque));
			infoBloque->bloqueNodo = bloques->bloque_nodo;
			infoBloque->bloqueArchivo = bloques->bloque_archivo;
			infoBloque->copia = bloques->copia;

			list_add(nodo->infoBloques, infoBloque);
			list_add(nodosDisponibles, nodo);
			log_trace(logYAMA, "Se agregó nodo %d con bloqueNodo %d, bloqueArchivo %d y copia %d", nodo->id, infoBloque->bloqueNodo, infoBloque->bloqueArchivo, infoBloque->copia);
		}

		void* data = receive(socketFS,&header);
		payload_UBICACION_BLOQUE* bloques = data;
	}
	log_trace(logYAMA, "Se cargaron los nodos correctamente");
	return nodosDisponibles;
}

void realizarTransformacion(t_job_master* job_master){
	t_list* nodosDisponibles = getNodosDeJob(job_master->job->id);
	int i,j, cantBloques;
	int cantNodos = list_size(nodosDisponibles);
	for(i=0; i<cantNodos;i++){
		t_worker* nodo = list_get(nodosDisponibles, i);
		nodoPasarAEtapa(nodo, TRANSFORMACION);
		cantBloques = list_size(nodo->bloquesAEjecutar);
		for(j=0; j<cantBloques; j++){
			t_infoBloque* bloque = list_get(nodo->bloquesAEjecutar,j);
			char* nombreArchivoTemporal = getNombreArchivoTemporalTransformacion(job_master->job->id, bloque->bloqueNodo, nodo->id);
			send_INFO_TRANSFORMACION(job_master->master_socket, nodo->puerto, nodo->ip, bloque->bloqueNodo, 1048576, nombreArchivoTemporal);
			actualizarTablaEstadosConTransformacion(job_master, nodo, bloque->bloqueNodo, nombreArchivoTemporal);
			aumentarCarga(nodo);
			log_trace(logYAMA, "Enviadas todas las transformaciones a los nodos disponibles");
		}
	}
}

void replanificar(t_job_master* job_master, t_worker* nodoFallido){

	t_list* listaNodos = getNodosDeJob(job_master->job->id);
	int cantidadBloquesAReplanificar = list_size(nodoFallido->bloquesAEjecutar);
	int i, j;

	int sacarNodoCaido(t_worker* worker){
		return worker->id != nodoFallido->id;
	}
	t_list* nodosActivos = list_filter(listaNodos, (void*)sacarNodoCaido);

	for(i=0; i < cantidadBloquesAReplanificar; i++){
		t_infoBloque* bloqueABuscar = list_get(nodoFallido->bloquesAEjecutar, i);
		int tieneCopia(t_infoBloque* bloqueInfo){
			return bloqueInfo->bloqueArchivo == bloqueABuscar->bloqueArchivo;
		}
		for(j=0; j < list_size(nodosActivos); j++){
			t_worker* worker = list_get(nodosActivos, j);
			if(list_any_satisfy(worker->infoBloques, (void*)tieneCopia)){
				t_infoBloque* bloqueEncontrado = list_find(worker->infoBloques, (void*)tieneCopia);
				char* nombreArchivoTemporal = getNombreArchivoTemporalTransformacion(job_master->job->id, bloqueEncontrado->bloqueNodo, worker->id);
				send_INFO_TRANSFORMACION(job_master->master_socket, worker->puerto, worker->ip, bloqueEncontrado->bloqueNodo, 1048576, nombreArchivoTemporal);
				actualizarTablaEstadosConTransformacion(job_master, worker, bloqueEncontrado->bloqueNodo, nombreArchivoTemporal);
			}
		}
	}

	int nodoConID(t_worker* nodo){
		return nodo->id == nodoFallido->id;
	}
	void eliminarNodo(t_worker* nodo){
		free(nodo);
	}
	list_remove_and_destroy_by_condition(listaNodos, (void*)nodoConID, (void*)eliminarNodo);
	list_destroy(nodosActivos);
}

t_infoBloque* buscarInfoBloque(t_list* bloques, int bloqueArchivo){
	int getBloque(t_infoBloque* infoBloque){
		return infoBloque->bloqueArchivo == bloqueArchivo;
	}
	return list_find(bloques, (void*) getBloque);
}

void aumentarCarga(t_worker* nodo){
	nodo->carga += 1;
}
void disminuirCarga(t_worker* nodo){
	nodo->carga -= 1;
}
void nodoPasarAEtapa(t_worker* nodo, Tarea etapa){
	nodo->etapaActiva = etapa;
}

void realizarReduccionLocal(t_worker* nodo, t_job_master* job_master){
	int getRegistroEstadoTransformacion(t_tablaEstados* registroEstado){
		return registroEstado->tarea == TRANSFORMACION &&
				registroEstado->nodo->id == nodo->id &&
				registroEstado->estado == EXITO &&
				registroEstado->job->id == job_master->job->id;
	}
	t_list* nodoConTransformacionTerminada = list_filter(TablaEstados, (void*)getRegistroEstadoTransformacion);
	char* nombreTemporalReduccionLocal = getNombreArchivoTemporalRedLocal(job_master->job->id, nodo->id);
	int cantNodos = list_size(nodoConTransformacionTerminada);
	int i;
	for(i=0; i < cantNodos; i++){
		t_tablaEstados* registro = list_get(nodoConTransformacionTerminada, i);
		send_INFO_REDUCCIONLOCAL(job_master->master_socket, nodo->puerto, nodo->ip , registro->archivoTemporal , nombreTemporalReduccionLocal);
		actualizarTablaEstadosConReduccion(job_master, nodo, nombreTemporalReduccionLocal, REDUCCION_LOCAL);
	}
	send_FIN_LISTA(job_master->master_socket);
	nodoPasarAEtapa(nodo, REDUCCION_LOCAL);
	aumentarCarga(nodo);
	list_destroy(nodoConTransformacionTerminada);
}

void realizarReduccionGlobal(t_job_master* job_master){
	t_worker* encargado = elegirEncargadoRedGlobal(job_master->job->id);

	int getRegistroEstadoRedLocal(t_tablaEstados* registroEstado){
		return registroEstado->tarea == REDUCCION_LOCAL &&
				registroEstado->estado == EXITO &&
				registroEstado->job->id == job_master->job->id;
	}
	t_list* nodosConRedLocalTerminada = list_filter(TablaEstados, (void*)getRegistroEstadoRedLocal);
	char* nombreTemporalReduccionGlobal = getNombreArchivoTemporalRedGlobal(job_master->job->id, job_master->master_id);
	void realizarRedGlobal(t_tablaEstados* registroEstado){
		send_INFO_REDUCCIONGLOBAL(job_master->master_socket, registroEstado->nodo->puerto, registroEstado->nodo->ip , registroEstado->archivoTemporal , nombreTemporalReduccionGlobal, encargado->id);
		actualizarTablaEstadosConReduccion(job_master, registroEstado->nodo, nombreTemporalReduccionGlobal, REDUCCION_GLOBAL);
		nodoPasarAEtapa(registroEstado->nodo, REDUCCION_GLOBAL);
		aumentarCarga(registroEstado->nodo);
	}
	list_iterate(nodosConRedLocalTerminada, (void*)realizarRedGlobal);
	send_FIN_LISTA(job_master->master_socket);
	list_destroy(nodosConRedLocalTerminada);
}

t_worker* elegirEncargadoRedGlobal(int jobID){
	t_list* listaNodos = getNodosDeJob(jobID);
	int ordenar(t_worker* nodo1, t_worker* nodo2){
		if(nodo1->carga == nodo2->carga){
			return nodo1->cantTareasHistoricas < nodo2->cantTareasHistoricas;
		}
		else {
			return nodo1->carga < nodo2->carga;
		}
	}
	list_sort(listaNodos, (void*)ordenar);
	return list_get(listaNodos, 0);
}

Tarea etapaActiva(t_worker* nodo){
	return nodo->etapaActiva;
}

int registroTerminoExitosamente(t_tablaEstados* registroEstado){
	return registroEstado->estado == EXITO;
}

int todosLosNodosTerminaronReduccionLocal(int jobID){

	t_list* nodosActivos = getNodosDeJob(jobID);

	int nodoConJOBYReduccionLocal(t_tablaEstados* registroEstado){
		return registroEstado->job->id == jobID && registroEstado->tarea == REDUCCION_LOCAL;
	}

	t_list* nodosEnReduccionLocal = list_filter(TablaEstados, (void*)nodoConJOBYReduccionLocal);

	if(list_size(nodosActivos) == list_size(nodosEnReduccionLocal)){ // La cantidad de nodos activos tiene que ser la misma que los nodos que estén en red local
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
	int registroEspecifico(t_tablaEstados* registroEstado){
		return registroEstado->nodo->id == infoMaster->id_nodo &&
				registroEstado->bloque == infoMaster->bloque &&
				registroEstado->estado == EN_EJECUCION &&
				registroEstado->master == infoMaster->id_master &&
				registroEstado->job->id == jobID;
	}
	return list_find(TablaEstados, (void*)registroEspecifico);
}

void actualizarEstados(payload_RESPUESTA_MASTER* infoMaster, t_job_master* job_master){
	actualizarTablaEstados(infoMaster, job_master);
	actualizarLog(infoMaster);

	t_worker* nodo = getNodo(infoMaster->id_nodo, job_master->job->id);
	disminuirCarga(nodo);
	aumentarTareasHistoricas(nodo);
}

void actualizarTablaEstados(payload_RESPUESTA_MASTER* infoMaster, t_job_master* job_master){
	t_tablaEstados* registroEstado = getRegistro(infoMaster, job_master->job->id);
	if(infoMaster->estado){
		registroEstado->estado = EXITO;
	}
	else {
		registroEstado->estado = ERROR;
	}
}

void actualizarLog(payload_RESPUESTA_MASTER* infoMaster){
	if(infoMaster->estado){
		log_trace(logYAMA, "Tarea en bloque %d de nodo %d OK", infoMaster->bloque, infoMaster->id_nodo);
	}
	else {
		log_error(logYAMA, "Tarea %d de nodo %d ERROR", infoMaster->bloque, infoMaster->id_nodo);
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
	log_trace(logYAMA, "Job %d - Nodo %d - Bloque %d - %s - %s", registroEstado->job->id, registroEstado->nodo->id, registroEstado->bloque, tarea, estado);
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
	char* tarea = castearTarea(registroEstado->tarea);
	char* estado = castearEstado(registroEstado->estado);
	log_trace(logYAMA, "Job %d - Nodo %d - %s - %s", registroEstado->job->id, registroEstado->nodo->id, tarea, estado);
}

t_worker* getNodo(int nodoID, int jobID){
	int nodoConId(t_worker* nodo){
		return nodo->id == nodoID;
	}
	t_list* nodosDisponibles = getNodosDeJob(jobID);
	return list_find(nodosDisponibles, (void*)nodoConId);
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
	t_list* listaNodos = getNodosDeJob(jobID);
	void maximo(t_infoBloque* bloque){
		if(bloque->bloqueArchivo > max){
			max = bloque->bloqueArchivo;
		}
	}
	void sumarTotal(t_worker* worker){
		list_iterate(worker->infoBloques, (void*)maximo);
	}
	list_iterate(listaNodos, (void*)sumarTotal);
	return max;
}


/*int main(void) {
	t_worker* worker1 = newNodo(1, 5041, "192.168.1");
	t_worker* worker2 = newNodo(2, 5042, "192.168.2");
	t_worker* worker3 = newNodo(3, 5043, "192.168.3");
	t_worker* worker4 = newNodo(4, 5044, "192.168.4");

	t_infoBloque* bloque1 = malloc(sizeof(t_infoBloque));
	bloque1->bloqueNodo = 10;
	bloque1->bloqueArchivo = 0;
	bloque1->copia = 0;
	t_infoBloque* bloque2 = malloc(sizeof(t_infoBloque));
	bloque2->bloqueNodo = 14;
	bloque2->bloqueArchivo = 0;
	bloque2->copia = 1;

	t_infoBloque* bloque3 = malloc(sizeof(t_infoBloque));
	bloque3->bloqueNodo = 8;
	bloque3->bloqueArchivo = 1;
	bloque3->copia = 0;
	t_infoBloque* bloque4 = malloc(sizeof(t_infoBloque));
	bloque4->bloqueNodo = 4;
	bloque4->bloqueArchivo = 1;
	bloque4->copia =1;

	t_infoBloque* bloque5 = malloc(sizeof(t_infoBloque));
	bloque5->bloqueNodo = 3;
	bloque5->bloqueArchivo = 2;
	bloque5->copia = 1;
	t_infoBloque* bloque6 = malloc(sizeof(t_infoBloque));
	bloque6->bloqueNodo = 6;
	bloque6->bloqueArchivo = 2;
	bloque6->copia = 0;

	t_infoBloque* bloque7 = malloc(sizeof(t_infoBloque));
	bloque7->bloqueNodo = 20;
	bloque7->bloqueArchivo = 3;
	bloque7->copia = 0;
	t_infoBloque* bloque8 = malloc(sizeof(t_infoBloque));
	bloque8->bloqueNodo = 15;
	bloque8->bloqueArchivo = 3;
	bloque8->copia = 1;

	list_add(worker1->infoBloques, bloque2);
	list_add(worker1->infoBloques, bloque6);

	list_add(worker2->infoBloques, bloque3);
	list_add(worker2->infoBloques, bloque8);

	list_add(worker3->infoBloques, bloque5);
	list_add(worker3->infoBloques, bloque7);

	list_add(worker4->infoBloques, bloque1);
	list_add(worker4->infoBloques, bloque4);

	return EXIT_SUCCESS;
}*/

void planificacionWClock(t_job_master* job_master){
	int i;
	t_list* listaNodos = getNodosDeJob(job_master->job->id);
	for(i=0; i < list_size(listaNodos); i++){
		t_worker* worker = list_get(listaNodos, i);
		calcularDisponibilidad(worker, job_master->job->id);
	}
	ordenarListaNodosPorDisponibilidad(listaNodos);

	t_worker* workerActual = malloc(sizeof(t_worker));
	t_link_element* valor = malloc(sizeof(t_link_element));
	int bloqueArchivo;
	int cantidadTotalBloquesArchivo = getTotalBloquesArchivo(job_master->job->id);
	valor = listaNodos->head;
	for(bloqueArchivo = 0; bloqueArchivo < cantidadTotalBloquesArchivo; bloqueArchivo++){

		while(1){
			workerActual = valor->data;
			if(existeEn(workerActual->infoBloques, bloqueArchivo) != 0){
				if(workerActual->disponibilidad > 0){
					t_infoBloque* infoBloque = buscarInfoBloque(workerActual->infoBloques, bloqueArchivo);
					list_add(workerActual->bloquesAEjecutar, infoBloque);
					workerActual->carga += 1;
					workerActual->disponibilidad -= 1;

					if(valor->next){
						valor = valor->next;
					}
					else{
						valor = listaNodos->head;
					}
					break;
				}
				else{
					workerActual->disponibilidad = base;
				}
			}
			if(valor->next){
				valor = valor->next;
			}
			else{
				valor = listaNodos->head;
			}
		}
	}
	realizarTransformacion(job_master);
	log_trace(logYAMA, "Planificacion terminada. Mandando a realizar instrucciones a los nodos");
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

int PWL(t_worker* worker, int jobID){
	return WLmax(jobID) + carga(worker);
}

int WLmax(int jobID){ // ordena la lista de nodos segun la disponibilidad
	t_list* listaNodos = getNodosDeJob(jobID);
	t_worker* worker = malloc(sizeof(t_worker));
	int mayorCarga(t_worker* nodo1, t_worker* nodo2){
			return carga(nodo1) > carga(nodo2);
		}
	list_sort(listaNodos,(void*)mayorCarga);
	//para verificar que el primero este bien
	worker = listaNodos->head->data;
	return carga(worker);
}/// VER COMO USAR LA LISTA SIN EL ID DE JOB; O DEBERIA USAR EL ID DE JOB? ES UNA FUNCION SOBRE UN WORKER GENERICO O TIENE EN CUENTA EL JOB?

int carga(t_worker* worker){
	return worker->carga;
}

void calcularDisponibilidad(t_worker* worker, int jobID){
	if(string_equals_ignore_case(configYAMA->algoritmoBalanceo,"WCLOCK")){
		worker->disponibilidad += PWL(worker, jobID);
	}
}

int disponibilidad(t_worker* worker){
	return worker->disponibilidad;
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
