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

int ESTAINICIALIZADO = 0;
int idUltimoJobCreado = 0;
int base = 2;

static t_list* nodosDisponibles;

void iniciarPlanificacion(char* nombreArchivo){
	usleep(configYAMA->retardoPlanificacion);
	int idJob = inicializarPlanificador();
	nodosDisponibles = obtenerNodosParaPlanificacion(nombreArchivo);//Funcion a desarrollar conjuntamente con FS
	planificacionWClock(nodosDisponibles, idJob);

	while(!todosLosNodosTerminaronReduccionLocal(idJob)){

		//Revisar semaforos productor consumidor
		payload_RESPUESTA_MASTER* infoMaster = obtenerSiguienteInfoMaster();

		if(infoMaster->estado){ //SI LA OPERACION FUE EXITOSA
			if(nodoTerminoTransformacion(infoMaster->id_nodo)){
				log_trace(logYAMA, "El nodo %d finalizo transformacion en todos sus bloques. Pasando a reduccion local", infoMaster->id_nodo);
				nodoPasarAReduccionLocal(infoMaster->id_nodo);
				realizarReduccionLocal(infoMaster->id_nodo);
			}
			else{
				t_worker* nodo = getNodo(infoMaster->id_nodo);
				nodo->cantTareasHistoricas += 1;
				log_trace(logYAMA, "El nodo %d finalizo transformacion en el bloque %d", infoMaster->id_nodo, infoMaster->bloque);
			}
		}
		else {
			replanificar(infoMaster);
		}
	}
	t_worker* encargado = elegirEncargadoReduccionGlobal(); //A desarrollar
	realizarReduccionGlobal(encargado); // A desarrollar
	finalizar(); // Como debe finalizar todo?
}

void finalizar(){
	list_destroy(listaNodos);
	list_destroy(listaRespuestasMaster);
	dictionary_destroy(diccionarioJobs);
	dictionary_destroy(bloques_ejecutados);
	printf("Planificacion terminada");
}

int inicializarPlanificador(){ //Devuelve el id del job creado
	if(ESTAINICIALIZADO){
		t_job* job = newJob();
		int idJob = agregarJob(job);
		return idJob;
	}
	else{
		listaNodos = list_create();
		//Habria que cargar la lista de nodos con su carga y disponibilidad
		listaRespuestasMaster = list_create();
		diccionarioJobs = dictionary_create();
		bloques_ejecutados = dictionary_create();
		t_job* job = newJob();
		int idJob = agregarJob(job);
		ESTAINICIALIZADO++;
		return idJob;
	}
}

int agregarJob(t_job* job){
	char* keyJob = string_itoa(job->id);
	dictionary_put(diccionarioJobs, keyJob, job);
	return job->id;
}

t_job *newJob()
{
	t_job *job = malloc(sizeof(t_job));
	idUltimoJobCreado++;
	job->id = idUltimoJobCreado;
	job->estado = EN_EJECUCION;
	job->etapa = TRANSFORMACION;
	return job;
}

void nodoPasarAReduccionLocal(int id){
	t_worker* nodo = getNodo(id);
	nodo->etapaActiva = REDUCCION_LOCAL;
}

int registroTerminoExitosamente(t_tablaEstados* registroEstado){
	return registroEstado->estado == EXITO;
}

int todosLosNodosTerminaronReduccionLocal(int idJob){

	int nodoConIDYReduccionLocal(t_tablaEstados* registroEstado){
		return registroEstado->job->id == idJob && registroEstado->tarea == REDUCCION_LOCAL;
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

char* getNombreArchivoTemporal(int jobId, int bloque, int etapa, int nodo){
	char* nombre = string_from_format("Job%d-Nodo%d-Bloque%d-Etapa%d",jobId,nodo,bloque,etapa);
	return nombre;
}

int* getSocketMasterId(int id_master){
	char* keyMaster = string_itoa(id_master);
	int* socketMaster = dictionary_get(diccionarioMasters, keyMaster);
	return socketMaster;
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
	if(infoMaster->estado){
		registroEstado->estado = EXITO;
	}
	else {
		registroEstado->estado = ERROR;
	}

	list_add(TablaEstados, registroEstado);
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

t_worker* getNodo(int id){
	int nodoConId(t_worker* nodo){
		return nodo->id == id;
	}
	return list_find(nodosDisponibles, (void*)nodoConId);
}

t_job* getJobDeNodo(int idNodo){
	int registroConNodoId(t_tablaEstados* registroEstado){
		return registroEstado->nodo->id == idNodo;
	}
	t_tablaEstados* registroEstado = list_find(TablaEstados, (void*)registroConNodoId);
	return registroEstado->job;
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

void planificacionWClock(t_list* listaNodos, int idJob){//Esta seria la lista o diccionario de workers
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
	realizarTransformacionNodos(idJob);
	log_trace(logYAMA, "Planificacion terminada. Mandando a realizar instrucciones a los nodos");
}

int getSocketMaster(int idJob){
	char* job = string_itoa(idJob);
	int* socket = dictionary_get(diccionarioMasters, job);
	return *socket;
}

void realizarTransformacionNodos(idJob){
	int socketMaster = getSocketMaster(idJob);
	int i,j;
	for(i=0; i<list_size(nodosDisponibles);i++){
		t_worker* nodo = list_get(nodosDisponibles, i);
		nodo->etapaActiva = TRANSFORMACION;
		for(j=0; j<list_size(nodo->bloquesAEjecutar);j++){
			char* bloqueChar = list_get(nodo->bloquesAEjecutar,j);
			uint16_t bloque = bloqueChar[0] - '0'; // Esto convierte el '1' a 1
			char* archivoTemporal = getNombreArchivoTemporal(idJob, bloque, TRANSFORMACION, nodo->id);
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

