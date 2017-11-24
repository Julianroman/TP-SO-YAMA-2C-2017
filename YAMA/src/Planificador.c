/*
 * Planificador.c
 *
 *  Created on: 20/10/2017
 *      Author: utnso
 */

#include <unistd.h>
#include <commons/string.h>
#include <utilidades/protocol/senders.h>
#include "Planificador.h"
#include "YAMA.h"

static int ESTAINICIALIZADO = 0;
static int idUltimoJobCreado = 0;
int base = 2;


void iniciarPlanificacion(char* nombreArchivo, t_job_master* job_master){
	usleep(configYAMA->retardoPlanificacion);
	inicializarPlanificador(job_master);
	cargarNodosParaPlanificacion(nombreArchivo);//Funcion a desarrollar conjuntamente con FS
	planificacionWClock(job_master);
}

void responderSolicitudMaster(payload_RESPUESTA_MASTER* infoMaster, t_job_master* job_master){

	actualizarEstados(infoMaster, job_master);

	t_worker* nodo = getNodo(infoMaster->id_nodo, job_master->job->id);

	if(infoMaster->estado == 0){ //SI LA OPERACION FUE EXITOSA

		switch(etapaActiva(nodo)){
		case TRANSFORMACION:
			if(nodoTerminoTransformacion(infoMaster->id_nodo)){
				log_trace(logYAMA, "El nodo %d finalizo transformacion en todos sus bloques. Pasando a reduccion local", infoMaster->id_nodo);
				realizarReduccionLocal(nodo, job_master->job->id);
				}
			else{
				nodo->cantTareasHistoricas += 1;
				log_trace(logYAMA, "El nodo %d finalizo transformacion en el bloque %d", infoMaster->id_nodo, infoMaster->bloque);
				}
			break;
		case REDUCCION_LOCAL:
			nodo->cantTareasHistoricas += 1;
			log_trace(logYAMA, "El nodo %d finalizo Reduccion Local", infoMaster->id_nodo);
			if(todosLosNodosTerminaronReduccionLocal(job_master->job->id)){
				/*t_worker* encargado = elegirEncargadoReduccionGlobal(jobID); A desarrollar
				realizarReduccionGlobal(encargado);  A desarrollar*/
			}
			break;
		case REDUCCION_GLOBAL:
			//QUE SE HACE?
			break;
		case ALMACENAMIENTO:
			// QUE SE HACE?
			break;
		}
	}

	else { // SI LA OPERACION FUE ERROR
		if(etapaActiva(nodo) == TRANSFORMACION){
			log_trace(logYAMA, "Replanificando transformacion para Nodo %d Bloque %d", infoMaster->id_nodo, infoMaster->bloque);
			replanificar(infoMaster, job_master->job->id);
		}
		else {
			log_error(logYAMA, "Fallo en etapa NO REPLANIFICABLE - Abortando JOB");
			abortarJob(job_master->job->id);
		}
	}

	finalizarCorrectamente(job_master->job->id);
}

void abortarJob(t_job* job){
	job->estado = ERROR;
	list_destroy(getNodosDeJob(job));
	log_trace(logYAMA, "JOB TERMINADO ERRONEAMENTE");
}

void finalizarCorrectamente(t_job* job){
	job->estado = EXITO;
	list_destroy(getNodosDeJob(job));
	log_trace(logYAMA, "JOB TERMINADO CORRECTAMENTE");
}

void inicializarPlanificador(t_job_master* job_master){ //Devuelve el id del job creado
	if(ESTAINICIALIZADO == 0){
		diccionarioJobNodos = dictionary_create();
		ESTAINICIALIZADO++;
	}
	nodosDisponibles = list_create();
	job_master->job = newJob();
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

void agregarListaNodosAJob(t_list* listaNodos, int jobID){
	char* keyJob = string_itoa(jobID);
	dictionary_put(diccionarioJobNodos, keyJob, listaNodos);
}

t_list* getNodosDeJob(int jobID){
	char* keyJob = string_itoa(jobID);
	return dictionary_get(diccionarioJobNodos, keyJob);
}

/*void replanificar(payload_RESPUESTA_MASTER* respuesta, int jobID){
	int nodoFallido(t_tablaEstados* registro){
		return registro->estado == ERROR && registro->nodo->id == respuesta->id_nodo && registro->tarea == TRANSFORMACION && registro->bloque == respuesta->bloque;
	}
	t_tablaEstados* registroEstado = list_find(TablaEstados, (void*)nodoFallido);

	int nodoConID(t_worker* nodo){
		return nodo->id == registroEstado->nodo->id;
	}
	void eliminarNodo(t_worker* nodo){
		free(nodo);
	}
	/*t_worker* nodo = getNodoConCopiaDeBloque(registroEstado->nodo, registroEstado->bloque);
	int bloque = getBloque
	t_list* nodosDisponibles = getNodosDeJob(jobID);
	list_remove_and_destroy_by_condition(nodosDisponibles, (void*)nodoConID, (void*)eliminarNodo);
	int socketMaster = getSocketMaster(registroEstado->job);
	send_INFO_TRANSFORMACION(socketMaster, nodo->puerto, nodo->ip, bloqueNuevo, 1048576, registroEstado->archivoTemporal);
}*/

void nodoPasarAReduccionLocal(t_worker* nodo){
	nodo->etapaActiva = REDUCCION_LOCAL;
}

void nodoPasarATransformacion(t_worker* nodo){
	nodo->etapaActiva = TRANSFORMACION;
}

void realizarReduccionLocal(t_worker* nodo, t_job_master* job_master){
	int getRegistroEstadoTransformacion(t_tablaEstados* registroEstado){
		return registroEstado->tarea == TRANSFORMACION &&
				registroEstado->nodo->id == nodo->id &&
				registroEstado->estado == EXITO &&
				registroEstado->job->id == job_master->job->id;
	}
	t_list* nodoConTransformacionTerminada = list_create();
	nodoConTransformacionTerminada = list_filter(TablaEstados, (void*)getRegistroEstadoTransformacion);
	char* nombreTemporalReduccionLocal = getNombreArchivoTemporalRedLocal(job_master->job->id, nodo->id);
	void realizarRedLocal(t_tablaEstados* registroEstado){
		send_INFO_REDUCCIONLOCAL(job_master->master_socket, nodo->puerto, nodo->ip , registroEstado->archivoTemporal , nombreTemporalReduccionLocal);
	}
	list_iterate(nodoConTransformacionTerminada, (void*)realizarRedLocal);
	send_FIN_LISTA(job_master->master_socket);
	nodoPasarAReduccionLocal(nodo);
	list_destroy(nodoConTransformacionTerminada);
}

Tarea etapaActiva(t_worker* nodo){
	return nodo->etapaActiva;
}

int registroTerminoExitosamente(t_tablaEstados* registroEstado){
	return registroEstado->estado == EXITO;
}

int todosLosNodosTerminaronReduccionLocal(int jobID){

	int nodoConJOBYReduccionLocal(t_tablaEstados* registroEstado){
		return registroEstado->job->id == jobID && registroEstado->tarea == REDUCCION_LOCAL;
	}

	t_list* nodosEnReduccionLocal = list_filter(TablaEstados, (void*)nodoConJOBYReduccionLocal);
	return list_all_satisfy(nodosEnReduccionLocal, (void*)registroTerminoExitosamente);
}

int nodoTerminoTransformacion(int idNodo){

	int nodoConIDYTransformacion(t_tablaEstados* registroEstado){
		return registroEstado->nodo->id == idNodo && registroEstado->tarea == TRANSFORMACION;
	}

	t_list* bloquesEnTransformacion = list_filter(TablaEstados, (void*)nodoConIDYTransformacion);
	return list_all_satisfy(bloquesEnTransformacion, (void*)registroTerminoExitosamente);
}

char* getNombreArchivoTemporalTransformacion(int jobID, int bloque, int nodo){
	char* nombre = string_from_format("Job%d-Nodo%d-Bloque%d-TRANSFORMACION",jobID,nodo,bloque);
	return nombre;
}

char* getNombreArchivoTemporalRedLocal(int jobID, int nodo){
	char* nombre = string_from_format("Job%d-Nodo%d-REDUCCION_LOCAL",jobID,nodo);
	return nombre;
}

void actualizarEstados(payload_RESPUESTA_MASTER* infoMaster, t_job_master* job_master){
	actualizarTablaEstados(infoMaster, job_master);
	actualizarLog(infoMaster);
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

void actualizarTablaEstados(payload_RESPUESTA_MASTER* infoMaster, t_job_master* job_master){
	t_tablaEstados* registroEstado = getRegistro(infoMaster, job_master->job->id);
	if(infoMaster->estado == 0){
		registroEstado->estado = EXITO;
	}
	else {
		registroEstado->estado = ERROR;
	}
}

void agregarRegistroATablaEstados(payload_RESPUESTA_MASTER* infoMaster, t_job_master* job_master){
	t_tablaEstados* registroEstado = malloc(sizeof(t_tablaEstados));
	registroEstado->job = job_master->job;
	registroEstado->master = infoMaster->id_master;
	registroEstado->nodo = getNodo(infoMaster->id_nodo,job_master->job->id); // El id de master es el mismo que el del job
	registroEstado->bloque = infoMaster->bloque;
	registroEstado->tarea = getTarea(infoMaster);
	registroEstado->archivoTemporal = getArchivoTemporal(infoMaster);
	if(infoMaster->estado == 0){
		registroEstado->estado = EXITO;
	}
	else {
		registroEstado->estado = ERROR;
	}

	list_add(TablaEstados, registroEstado);
	log_trace(logYAMA, "Job %d - Master %d - Nodo %d - Bloque %d - Tarea %s - Archivo Temporal %s - Estado %s",
			registroEstado->job->id, registroEstado->master, registroEstado->nodo->id, registroEstado->bloque, registroEstado->tarea,
			registroEstado->archivoTemporal, registroEstado->estado);
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
	registroEstado->nodo = nodo; // El id de master es el mismo que el del job
	registroEstado->bloque = bloque;
	registroEstado->tarea = TRANSFORMACION;
	registroEstado->archivoTemporal = nombreArchivoTemporal;
	registroEstado->estado = EN_EJECUCION;

	list_add(TablaEstados, registroEstado);
	log_trace(logYAMA, "Job %d - Master %d - Nodo %d - Bloque %d - Tarea %s - Archivo Temporal %s - Estado %s",
			registroEstado->job->id, registroEstado->master, registroEstado->nodo->id, registroEstado->bloque, registroEstado->tarea,
			registroEstado->archivoTemporal, registroEstado->estado);
}

t_worker* getNodo(int nodoID, int jobID){
	int nodoConId(t_worker* nodo){
		return nodo->id == nodoID;
	}
	t_list* nodosDisponibles = getNodosDeJob(jobID);
	return list_find(nodosDisponibles, (void*)nodoConId);
}

Tarea getTarea(payload_RESPUESTA_MASTER* infoMaster){
	int registroEspecifico(t_tablaEstados* registroEstado){
			return registroEstado->nodo->id == infoMaster->id_nodo && registroEstado->bloque == infoMaster->bloque
					&& registroEstado->estado == infoMaster->estado && registroEstado->master == infoMaster->id_master;
		}
	t_tablaEstados* registroEstado = list_find(TablaEstados, (void*)registroEspecifico);
	return registroEstado->tarea;
}

char* getArchivoTemporal(payload_RESPUESTA_MASTER* infoMaster){
	int registroEspecifico(t_tablaEstados* registroEstado){
				return registroEstado->nodo->id == infoMaster->id_nodo && registroEstado->bloque == infoMaster->bloque
						&& registroEstado->estado == infoMaster->estado && registroEstado->master == infoMaster->id_master;
			}
	t_tablaEstados* registroEstado = list_find(TablaEstados, (void*)registroEspecifico);
	return registroEstado->archivoTemporal;
}

/*typedef struct {
	char *nombre;
	char *ip;
	char *puerto;
	int carga;
	t_list * bloquesMios;
	t_list * bloquesAdquiridos;
} t_worker;

t_list* listaWorkers;
t_worker* worker1;
t_worker* worker2;
t_worker* worker3;

int main(void) {
	iniciarPlanificador();
	worker1 = malloc(sizeof(t_worker));
	worker2 = malloc(sizeof(t_worker));
	worker3 = malloc(sizeof(t_worker));
	worker1->id = 1;
	worker1->cantTareasHistoricas = 2;
	worker1->carga = 1;
	worker1->bloquesDelDatanode = list_create();
	worker2->bloquesDelDatanode = list_create();
	worker3->bloquesDelDatanode = list_create();
	worker1->bloquesAEjecutar = list_create();
	worker2->bloquesAEjecutar = list_create();
	worker3->bloquesAEjecutar = list_create();
	list_add(worker1->bloquesDelDatanode, "0");
	list_add(worker1->bloquesDelDatanode, "1");
	list_add(worker1->bloquesDelDatanode, "3");
	list_add(worker1->bloquesDelDatanode, "4");
	list_add(worker1->bloquesDelDatanode, "6");
	worker2->id = 2;
	worker2->cantTareasHistoricas = 1;
	worker2->carga = 1;
	list_add(worker2->bloquesDelDatanode, "0");
	list_add(worker2->bloquesDelDatanode, "2");
	list_add(worker2->bloquesDelDatanode, "3");
	list_add(worker2->bloquesDelDatanode, "5");
	list_add(worker2->bloquesDelDatanode, "6");
	worker3->id = 3;
	worker3->cantTareasHistoricas = 3;
	worker3->carga = 0;
	list_add(worker3->bloquesDelDatanode, "1");
	list_add(worker3->bloquesDelDatanode, "2");
	list_add(worker3->bloquesDelDatanode, "4");
	list_add(worker3->bloquesDelDatanode, "5");
	list_add(listaNodos, worker1);
	list_add(listaNodos, worker2);
	list_add(listaNodos, worker3);
	calcularDisponibilidad(worker1);
	calcularDisponibilidad(worker2);
	calcularDisponibilidad(worker3);
	nodoConMayorDisponibilidad();
 	planificacion(listaNodos);
 	planificacion(listaNodos);
	return EXIT_SUCCESS;
}*/

void planificacionWClock(t_job_master* job_master){//Esta seria la lista o diccionario de workers

	t_list* listaNodos = getNodosDeJob(job_master->job->id);
	list_iterate(listaNodos, (void*)calcularDisponibilidad);
	nodoConMayorDisponibilidad(listaNodos);

	t_worker* workerMin = malloc(sizeof(t_worker));
	workerMin = listaNodos->head->data;
	t_worker* workerActual = malloc(sizeof(t_worker));
	t_link_element* valor = malloc(sizeof(t_link_element));
	int verificador = 0;
	int i;
	char *bloquesTotales[7] = {"0","1","2","3","4","5","6"};
	valor = listaNodos->head;
	int length = sizeof(bloquesTotales)/sizeof(char*);
	for(i = 0; i < length;i++){
		while(1){
			workerActual = valor->data;
			if(existeEn(workerActual->infoBloques, bloquesTotales[i]) != 0){
				if(workerActual->disponibilidad > 0){
					list_add(workerActual->bloquesAEjecutar, bloquesTotales[i]);
					workerActual->carga += 1;
					workerActual->disponibilidad -= 1;
					if(valor->next){
						valor = valor->next;
						verificador = 0;
					}else{
						verificador = 1;
					}
					if(verificador == 1)
						valor = listaNodos->head;
					break;
				}else{
					workerActual->disponibilidad = base;
				}
			}
			if(valor->next){
				valor = valor->next;
				verificador = 0;
			}else{
				verificador = 1;
			}
			if(verificador == 1)
				valor = listaNodos->head;
		}
	}
	realizarTransformacionNodos(job_master);
	log_trace(logYAMA, "Planificacion terminada. Mandando a realizar instrucciones a los nodos");
}

int charToInt(char* c){
	return atoi(c);
}

void realizarTransformacionNodos(t_job_master* job_master){
	t_list* nodosDisponibles = getNodosDeJob(job_master->job->id);
	int i,j;
	for(i=0; i<list_size(nodosDisponibles);i++){
		t_worker* nodo = list_get(nodosDisponibles, i);
		nodoPasarATransformacion(nodo);
		//Aca paso la lista de chars a ints
		nodo->bloquesAEjecutar = list_map(nodo->bloquesAEjecutar, (void*) charToInt);
		for(j=0; j<list_size(nodo->bloquesAEjecutar);j++){
			int bloque = list_get(nodo->bloquesAEjecutar,j);
			//char* bloqueChar = list_get(nodo->bloquesAEjecutar,j);
			//uint16_t bloque = bloqueChar[0] - '0'; // Esto convierte el '1' a 1
			char* nombreArchivoTemporal = getNombreArchivoTemporalTransformacion(job_master->job->id, bloque, nodo->id);
			send_INFO_TRANSFORMACION(job_master->master_socket, nodo->puerto, nodo->ip, bloque, 1048576, nombreArchivoTemporal);
			actualizarTablaEstadosConTransformacion(job_master, nodo, bloque, nombreArchivoTemporal);
		}
	}
}

int existeEn(t_list* lista , char* dato){
	int existeBloque(t_infoBloque* infoBloque){
		char* d = string_itoa(infoBloque->bloqueNodo);
		return string_equals_ignore_case(d, dato);
	}
	return list_any_satisfy(lista, (void*) existeBloque);
}

void nodoConMayorDisponibilidad(t_list* listaNodos){ // ordena la lista de nodos segun la disponibilidad
	t_worker* worker = malloc(sizeof(t_worker));
	int mayorDisponibilidad(t_worker* nodo1, t_worker* nodo2){
		if(disponibilidad(nodo1) == disponibilidad(nodo2)){
			return tareasHistoricas(nodo1) < tareasHistoricas(nodo2);
		}
		else
			return disponibilidad(nodo1) > disponibilidad(nodo2);
		}
	list_sort(listaNodos,(void*)mayorDisponibilidad);
	//para verificar que el primero este bien
	worker = listaNodos->head->data;
}

int PWL(t_worker* worker){
	return WLmax() + carga(worker);
}

/*int WLmax(){ // ordena la lista de nodos segun la disponibilidad
	t_worker* worker = malloc(sizeof(t_worker));
	int mayorCarga(t_worker* nodo1, t_worker* nodo2){
			return carga(nodo1) > carga(nodo2);
		}
	list_sort(listaNodos,(void*)mayorCarga);
	//para verificar que el primero este bien
	worker = listaNodos->head->data;
	return carga(worker);
}*/// VER COMO USAR LA LISTA SIN EL ID DE JOB; O DEBERIA USAR EL ID DE JOB? ES UNA FUNCION SOBRE UN WORKER GENERICO O TIENE EN CUENTA EL JOB?

int carga(t_worker* worker){
	return worker->carga;
}

void calcularDisponibilidad(t_worker* worker){
	if(string_equals_ignore_case(configYAMA->algoritmoBalanceo,"WCLOCK")){
		worker->disponibilidad = base + PWL(worker);
	}
	else{
		worker->disponibilidad = base;
	}
}

int disponibilidad(t_worker* worker){
	return worker->disponibilidad;
}

int tareasHistoricas(t_worker* worker){
	return worker->cantTareasHistoricas;
}

int estaActivo(t_worker* worker){
	return worker->activo == 1;
}
