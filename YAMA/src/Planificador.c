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

static t_list* nodosDisponibles;

void iniciarPlanificacion(char* nombreArchivo){
	usleep(configYAMA->retardoPlanificacion);
	int jobID = inicializarPlanificador();
	nodosDisponibles = obtenerNodosParaPlanificacion(nombreArchivo);//Funcion a desarrollar conjuntamente con FS
	planificacionWClock(nodosDisponibles, jobID);

	while(!todosLosNodosTerminaronReduccionLocal(jobID)){

		//Revisar semaforos productor consumidor
		payload_RESPUESTA_MASTER* infoMaster = obtenerSiguienteInfoMaster();

		if(infoMaster->estado == 0){ //SI LA OPERACION FUE EXITOSA

			if(etapaActiva(infoMaster->id_nodo) == TRANSFORMACION){
				if(nodoTerminoTransformacion(infoMaster->id_nodo)){
					log_trace(logYAMA, "El nodo %d finalizo transformacion en todos sus bloques. Pasando a reduccion local", infoMaster->id_nodo);
					realizarReduccionLocal(infoMaster->id_nodo, jobID);
				}
				else{
					t_worker* nodo = getNodo(infoMaster->id_nodo);
					nodo->cantTareasHistoricas += 1;
					log_trace(logYAMA, "El nodo %d finalizo transformacion en el bloque %d", infoMaster->id_nodo, infoMaster->bloque);
				}
			}
			else {
				t_worker* nodo = getNodo(infoMaster->id_nodo);
				nodo->cantTareasHistoricas += 1;
				log_trace(logYAMA, "El nodo %d finalizo Reduccion Local", infoMaster->id_nodo);
			}
		}
		else { // SI LA OPERACION FUE ERROR
			if(etapaActiva(infoMaster->id_nodo) == TRANSFORMACION){
				log_trace(logYAMA, "Replanificando transformacion para Nodo %d Bloque %d", infoMaster->id_nodo, infoMaster->bloque);
				replanificar(infoMaster);
			}
			else {
				log_error(logYAMA, "Fallo en la reduccion local - Abortando JOB");
				abortarJob(jobID);
			}
		}
	}
	t_worker* encargado = elegirEncargadoReduccionGlobal(); //A desarrollar
	realizarReduccionGlobal(encargado); // A desarrollar
	finalizarCorrectamente();
}

void abortarJob(int jobID){
	t_job* job = getJob(jobID);
	job->estado = ERROR;
	list_destroy(listaNodos);
	list_destroy(listaRespuestasMaster);
	log_trace(logYAMA, "JOB TERMINADO ERRONEAMENTE");
}

void finalizarCorrectamente(int jobID){
	t_job* job = getJob(jobID);
	job->estado = EXITO;
	list_destroy(listaNodos);
	list_destroy(listaRespuestasMaster);
	log_trace(logYAMA, "JOB TERMINADO CORRECTAMENTE");
}

int inicializarPlanificador(){ //Devuelve el id del job creado
	if(ESTAINICIALIZADO == 0){
		diccionarioJobs = dictionary_create();
		ESTAINICIALIZADO++;
	}
	listaRespuestasMaster = list_create();
	listaNodos = list_create();
	t_job* job = newJob();
	agregarJob(job);
	return job->id;
}

int agregarJob(t_job* job){
	char* keyJob = string_itoa(job->id);
	dictionary_put(diccionarioJobs, keyJob, job);
	return job->id;
}

t_job* getJob(int jobID){
	char* keyJob = string_itoa(jobID);
	t_job* job = dictionary_get(keyJob);
	return job;
}

t_job *newJob(){
	t_job *job = malloc(sizeof(t_job));
	idUltimoJobCreado++;
	job->id = idUltimoJobCreado;
	job->estado = EN_EJECUCION;
	job->etapa = TRANSFORMACION;
	return job;
}

void replanificar(payload_RESPUESTA_MASTER* respuesta){
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
	int bloque = getBloque*/
	list_remove_and_destroy_by_condition(nodosDisponibles, (void*)nodoConID, (void*)eliminarNodo);
	int socketMaster = getSocketMaster(registroEstado->job);
	send_INFO_TRANSFORMACION(socketMaster, nodo->puerto, nodo->ip, bloqueNuevo, 1048576, registroEstado->archivoTemporal);
}

void nodoPasarAReduccionLocal(t_worker* nodo){
	nodo->etapaActiva = REDUCCION_LOCAL;
	nodo->jobActivo->etapa = REDUCCION_LOCAL;
}

void realizarReduccionLocal(int nodoID, int jobID){
	int socketMaster = getSocketMaster(jobID);
	t_worker* nodo = getNodo(nodoID);
	int getRegistroEstadoTransformacion(t_tablaEstados* registroEstado){
		return registroEstado->tarea == TRANSFORMACION && registroEstado->nodo->id == nodo->id && registroEstado->estado == EXITO;
	}
	t_list* nodoConTransformacionTerminada = list_find(TablaEstados, (void*)getRegistroEstadoTransformacion);
	char* nombreTemporalReduccionLocal = getNombreArchivoTemporalRedLocal(jobID, nodoID);
	void realizarRedLocal(t_tablaEstados* registroEstado){
		send_INFO_REDUCCIONLOCAL(socketMaster, nodo->puerto, nodo->ip , registroEstado->archivoTemporal , nombreTemporalReduccionLocal);
	}
	list_iterate(nodoConTransformacionTerminada, (void*)realizarRedLocal);
	nodoPasarAReduccionLocal(nodo);
}

Tarea etapaActiva(int nodoID){
	t_worker* nodo = getNodo(nodoID);
	return nodo->etapaActiva;
}

int registroTerminoExitosamente(t_tablaEstados* registroEstado){
	return registroEstado->estado == EXITO;
}

int todosLosNodosTerminaronReduccionLocal(int nodoID, int jobID){

	int nodoConIDYReduccionLocal(t_tablaEstados* registroEstado){
		return registroEstado->job->id == jobID && registroEstado->tarea == REDUCCION_LOCAL && registroEstado->nodo->id == nodoID;
	}

	t_list* nodosEnReduccionLocal = list_filter(TablaEstados, (void*)nodoConIDYReduccionLocal);
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

payload_RESPUESTA_MASTER* obtenerSiguienteInfoMaster(){
	payload_RESPUESTA_MASTER* infoMaster = list_remove(listaRespuestasMaster, 0); // Lo retorna y después lo remueve de la lista, así siempre si saco el primero de la lista es una instruccion que nunca saqué
	actualizarEstados(infoMaster);
	return infoMaster;
}

void actualizarEstados(payload_RESPUESTA_MASTER* infoMaster){
	actualizarTablaEstados(infoMaster);
	actualizarLog(infoMaster);
	actualizarEstadosNodo(infoMaster);
}

void actualizarTablaEstados(payload_RESPUESTA_MASTER* infoMaster){
	t_tablaEstados* registroEstado = malloc(sizeof(t_tablaEstados));
	registroEstado->job = getJobDeNodo(infoMaster->id_nodo);
	registroEstado->master = infoMaster->id_master;
	registroEstado->nodo = getNodo(infoMaster->id_nodo);
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

void actualizarEstadosNodo(payload_RESPUESTA_MASTER* infoMaster){
	if(infoMaster->estado){
		t_worker* nodo = getNodo(infoMaster->id_nodo);
		nodo->cantTareasHistoricas++;
	}
}

t_worker* getNodo(int nodoID){
	int nodoConId(t_worker* nodo){
		return nodo->id == nodoID;
	}
	return list_find(nodosDisponibles, (void*)nodoConId);
}

t_job* getJobDeNodo(int idNodo){
	/*int registroConNodoId(t_tablaEstados* registroEstado){
		return registroEstado->nodo->id == idNodo;
	}
	t_tablaEstados* registroEstado = list_find(TablaEstados, (void*)registroConNodoId);
	return registroEstado->job;*/
	t_worker* nodo = getNodo(idNodo);
	return nodo->jobActivo;
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

void planificacionWClock(t_list* listaNodos, int jobID){//Esta seria la lista o diccionario de workers

	list_iterate(nodosDisponibles, (void*)calcularDisponibilidad);
	nodoConMayorDisponibilidad();

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
			if(existeEn(workerActual->bloquesDelDatanode, bloquesTotales[i]) != 0){
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
	realizarTransformacionNodos(jobID);
	log_trace(logYAMA, "Planificacion terminada. Mandando a realizar instrucciones a los nodos");
}

int getSocketMaster(int jobID){
	char* job = string_itoa(jobID);
	return dictionary_get(diccionarioMasters, job);
}

int charToInt(char* c){
	return atoi(c);
}

void realizarTransformacionNodos(int jobID){
	int socketMaster = getSocketMaster(jobID);
	int i,j;
	for(i=0; i<list_size(nodosDisponibles);i++){
		t_worker* nodo = list_get(nodosDisponibles, i);
		nodo->etapaActiva = TRANSFORMACION;
		//Aca paso la lista de chars a ints
		nodo->bloquesAEjecutar = list_map(nodo->bloquesAEjecutar, (void*) charToInt);
		for(j=0; j<list_size(nodo->bloquesAEjecutar);j++){
			int bloque = list_get(nodo->bloquesAEjecutar,j);
			//char* bloqueChar = list_get(nodo->bloquesAEjecutar,j);
			//uint16_t bloque = bloqueChar[0] - '0'; // Esto convierte el '1' a 1
			char* archivoTemporal = getNombreArchivoTemporalTransformacion(jobID, bloque, nodo->id);
			send_INFO_TRANSFORMACION(socketMaster, nodo->puerto, nodo->ip, bloque, 1048576, archivoTemporal);
		}
	}
}

int existeEn(t_list* lista , char* dato){
	int existeBloque(char* d){
			 return string_equals_ignore_case(d, dato);
	}
	return list_find(lista, (void*) existeBloque);
}

void nodoConMayorDisponibilidad(){ // ordena la lista de nodos segun la disponibilidad
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

int WLmax(){ // ordena la lista de nodos segun la disponibilidad
	t_worker* worker = malloc(sizeof(t_worker));
	int mayorCarga(t_worker* nodo1, t_worker* nodo2){
			return carga(nodo1) > carga(nodo2);
		}
	list_sort(listaNodos,(void*)mayorCarga);
	//para verificar que el primero este bien
	worker = listaNodos->head->data;
	return carga(worker);
}

int carga(t_worker* worker){
	return worker->carga;
}

void calcularDisponibilidad(t_worker* worker){
	if(configYAMA->algoritmoBalanceo == "WCLOCK"){
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

void agregarAListaInfoMaster(payload_RESPUESTA_MASTER* infoMaster){
	list_add(listaRespuestasMaster, infoMaster);
}

