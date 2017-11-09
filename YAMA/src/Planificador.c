/*
 * Planificador.c
 *
 *  Created on: 20/10/2017
 *      Author: utnso
 */

#include <unistd.h>
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
	planificacionWClock(nodosDisponibles);
	while(!todosLosNodosTerminaronReduccionLocal(idJob)){
		//Revisar semaforos productor consumidor
		payload_RESPUESTA_MASTER* infoMaster = obtenerSiguienteInfoMaster(); //Master me encola todas las respuestas que tuvo de los workers - Devuelve el worker que necesita siguiente instruccion
		realizarSiguienteinstruccion(infoMaster);
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

int inicializarPlanificador(){ //Devuelve el id del job nuevo
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
	idUltimoJobCreado++;
	job->id = idUltimoJobCreado;
	char* keyJob = string_itoa(job->id);
	dictionary_put(diccionarioJobs, keyJob, job);
	return job->id;
}

t_job *newJob()
{
	t_job *job = malloc(sizeof(t_job));
	job->id = 0;
	job->estado = EN_EJECUCION;
	job->etapa = TRANSFORMACION;
	return job;
}

int todosLosNodosTerminaronReduccionLocal(t_list* nodosDisponibles){
	return list_all_satisfy(nodosDisponibles, (void*) nodoTerminoReduccionLocal);
}

int nodoTerminoReduccionLocal(t_worker* nodo){
		return (tareaEsReduccionLocal(nodo->tareaActiva) && tareaEstaFinalizada(nodo->tareaActiva)) || tareaEsReduccionGlobal(nodo->tareaActiva);
	}

int todosLosNodosTerminaronTransformacion(t_list* nodosDisponibles){
	int nodoTerminoTransformacion(t_worker* nodo){
		return (tareaEsTransformacion(nodo->tareaActiva) && tareaEstaFinalizada(nodo->tareaActiva)) || tareaEsReduccionLocal(nodo->tareaActiva);
	}
	return list_all_satisfy(nodosDisponibles, (void*) nodoTerminoTransformacion);
}

void realizarSiguienteinstruccion(payload_RESPUESTA_MASTER* respuesta){
	t_worker* worker = buscarNodo(nodosDisponibles, respuesta->id_nodo);
	t_tarea* tareaEjecutada =  getTarea(worker);
	if(!respuesta->estado){ // Si fue error
		actualizarEstados(respuesta);
		if(tareaEsReduccionLocal(tareaEjecutada) || tareaEsReduccionGlobal(tareaEjecutada)){
			abortarJob(); //A desarrollar
		}
		else{
			rePlanificarTransformacion(respuesta->id_nodo); // Debe mandar a hacer transformacion en donde está la copia de ese bloque
		}
	}
	else { //EJECUCION_OK
		actualizarEstados(respuesta);
		realizarSiguienteTarea(respuesta); //A desarrollar
	}
}

void realizarSiguienteTarea(payload_RESPUESTA_MASTER* respuesta){
	t_worker* worker = buscarNodo(nodosDisponibles, respuesta->id_nodo);
	if(tareaEsTransformacion(worker->tareaActiva)){
		//Mandar a hacer reduccion local
		int* socketMaster = getSocketMasterId(respuesta->id_master);
		char* nombreTemporal_transformacion = tareaObtenerNombreResultadoTemporal(worker->tareaActiva);
		tareaPasarAReduccionLocal(worker->tareaActiva);
		send_INFO_REDUCCIONLOCAL(*socketMaster, worker->puerto, worker->ip, nombreTemporal_transformacion, worker->tareaActiva->nombreResultadoTemporal);
	}
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
	registroEstado->job = getJobDeWorker(infoMaster->id_nodo);
	registroEstado->master = infoMaster->id_master;
	registroEstado->nodo = infoMaster->id_nodo;
	registroEstado->bloque = infoMaster->bloque;
	registroEstado->etapa = getTipoTarea(getTarea(buscarNodo(nodosDisponibles, infoMaster->id_nodo)));
	registroEstado->archivoTemporal = tareaObtenerNombreResultadoTemporal(getTarea(infoMaster->id_nodo));
	registroEstado->estado = infoMaster->estado;
	list_add(TablaEstados, registroEstado);
}

void actualizarLog(payload_RESPUESTA_MASTER* infoMaster){
	t_worker* worker = buscarNodo(nodosDisponibles, infoMaster->id_nodo);
	t_tarea* tareaEJecutada = getTarea(worker);
	if(infoMaster->estado){
		log_trace(logYAMA, "Tarea %s de worker %d OK", tareaEJecutada->tipo, infoMaster->id_nodo);
	}
	else {
		log_error(logYAMA, "Tarea %s de worker %d ERROR", tareaEJecutada->tipo, infoMaster->id_nodo);
	}
}

void actualizarEstadosNodo(payload_RESPUESTA_MASTER* infoMaster){
	t_worker* worker = buscarNodo(nodosDisponibles, infoMaster->id_nodo);
	t_tarea* tareaEJecutada = getTarea(worker);
	if(infoMaster->estado){ // Si es 1 significa que fue OK la ejecucion ahí recien marco como finalizada la tarea
		tareaMarcarFinalizada(worker->tareaActiva);
	}
}

t_job* getJobDeWorker(int id){
	t_worker* worker = buscarNodo(nodosDisponibles, id);
	return worker->jobActivo;
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

void planificacionWClock(t_list* listaNodos){//Esta seria la lista o diccionario de workers
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
	printf("Clock Terminaado");
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

t_worker* buscarNodo(t_list* listaNodos, int numNodo){
	int nodoConNombre(t_worker* nodo){
		return nodo->id == numNodo;
	}
	return list_find(listaNodos, (void*) nodoConNombre);
}

int estaActivo(t_worker* worker){
	return worker->activo == 1;
}

void agregarAListaInfoMaster(payload_RESPUESTA_MASTER* infoMaster){
	list_add(listaRespuestasMaster, infoMaster);
}

t_tarea* getTarea(t_worker* worker){
	return worker->tareaActiva;
}
