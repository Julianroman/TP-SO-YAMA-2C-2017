/*
 * Planificador.c
 *
 *  Created on: 20/10/2017
 *      Author: utnso
 */

#include <utilidades/protocol/types.h>
#include "Planificador.h"
#include "Job.h"
#include "YAMA.h"
#include "Tarea.h"

int ESTAINICIALIZADO = 0;
int idUltimoJobCreado = 0;
int idUltimaTareaCreada = 0;
int base = 2;

void iniciarPlanificacion(char* nombreArchivo){
	inicializarPlanificador();
	t_list* nodosDisponibles = obtenerNodosParaPlanificacion(nombreArchivo); //Funcion a desarrollar conjuntamente con FS
	planificacionWClock(&nodosDisponibles);
	while(!todosLosNodosTerminaronReduccionLocal(&nodosDisponibles)){
		payload_RESPUESTA_MASTER* infoMaster = obtenerSiguienteInfoMaster(); //Master me encola todas las respuestas que tuvo de los workers - Devuelve el worker que necesita siguiente instruccion
		realizarSiguienteInstruccion(&infoMaster);
	}
	t_worker* encargado = elegirEncargadoReduccionGlobal(&nodosDisponibles); //A desarrollar
	realizarReduccionGlobal(&encargado); // A desarrollar
	finalizar(); // Como debe finalizar todo?
}

void inicializarPlanificador(){
	if(ESTAINICIALIZADO){
		t_job* job = newJob();
		agregarJob(&job);
	}
	else{
		listaNodos = list_create();
		//Habria que cargar la lista de nodos con su carga y disponibilidad
		listaRespuestasMaster = list_create();
		diccionarioJobs = dictionary_create();
		diccionarioTareas = dictionary_create();
		bloques_ejecutados = dictionary_create();
		tablaEstados = list_create();
		ESTAINICIALIZADO++;
	}
}

void agregarJob(t_job* job){
	idUltimoJobCreado++;
	job->id = idUltimoJobCreado;
	char* keyJob = string_itoa(job->id);
	dictionary_put(diccionarioJobs, keyJob, job);
}

void agregarTarea(t_tarea* tarea){
	idUltimaTareaCreada++;
	tarea->id = idUltimaTareaCreada;
	char* keyTarea = string_itoa(tarea->id);
	dictionary_put(diccionarioJobs, keyTarea, tarea);
}

t_job *newJob()
{
	t_job *job = malloc(sizeof(t_job));
	job->id = 0;
	job->estado = EN_EJECUCION;
	return job;
}

int todosLosNodosTerminaronReduccionLocal(t_list* nodosDisponibles){
	int nodoTerminoReduccionLocal(t_worker* nodo){
		return tareaEstaFinalizada(nodo->jobActivo->reduccion_local);
	}
	return list_all_satisfy(nodosDisponibles, (void*) nodoTerminoReduccionLocal);
}

int todosLosNodosTerminaronTransformacion(t_list* nodosDisponibles){
	int nodoTerminoTransformacion(t_worker* nodo){
		return tareaEstaFinalizada(nodo->jobActivo->transformacion);
	}
	return list_all_satisfy(nodosDisponibles, (void*) nodoTerminoTransformacion);
}

void realizarSiguienteinstruccion(payload_RESPUESTA_MASTER* respuesta){
	t_tarea* tareaEjecutada = obtenerUltimaTareaEjecutadaPorWorker(respuesta->id_nodo); // A desarrollar
	if(!respuesta->estado){ // Si fue error
		actualizarEstados(&respuesta);
		if(tareaEsReduccionLocal(&tareaEjecutada) || tareaEsReduccionGlobal(&tareaEjecutada)){
			abortarJob(); //A desarrollar
		}
		else{
			rePlanificarTransformacion(respuesta->id_nodo); // Si el error se da en la tarea de trasnformacion como se vuelve a planificar?
		}
	}
	else { //EJECUCION_OK
		actualizarEstados(&respuesta);
		realizarSiguienteTarea(respuesta->id_nodo); //A desarrollar
	}
}

payload_RESPUESTA_MASTER* obtenerSiguienteInfoMaster(){

	payload_RESPUESTA_MASTER* infoMaster = list_remove(listaRespuestasMaster, 0); // Lo retorna y después lo remueve de la lista, así siempre si saco el primero de la lista es una instruccion que nunca saqué
	actualizarEstados(&infoMaster);
	return infoMaster;
}

void actualizarEstados(payload_RESPUESTA_MASTER* infoMaster){
	actualizarTablaestados(&infoMaster);
	actualizarLog(&infoMaster);
	actualizarEstadosNodo(&infoMaster);
}

void actualizarTablaEstados(payload_RESPUESTA_MASTER* infoMaster){
	t_tablaEstados* registroEstado = malloc(sizeof(t_tablaEstados));
	registroEstado->job = obtenerJobActivoPara(infoMaster->id_nodo);//A desarrollar
	registroEstado->master = infoMaster->id_master;
	registroEstado->nodo = infoMaster->id_nodo;
	registroEstado->bloque = infoMaster->bloque;
	registroEstado->etapa = obtenerUltimaTareaEjecutadaPara(infoMaster->id_nodo);//A desarrollar
	registroEstado->archivoTemporal = obtenerUltimoArchivoTemporalPara(infoMaster->id_nodo); //A desarrollar
	registroEstado->estado = infoMaster->estado;
	list_add(tablaEstados, registroEstado);
}

void actualizarLog(payload_RESPUESTA_MASTER* infoMaster){
	//A desarrollar
}

void actualizarEstadosNodo(payload_RESPUESTA_MASTER* infoMaster){
	t_worker* worker = obtenerNodo(infoMaster->id_nodo);
	//Pensar sobre si el worker tiene una tarea activa
	/*if(tareaEsTransformacion(respuesta->tareaEjecutada)){
		if(respuesta->estadoEjecucion == EJECUCION_OK){
			tareaMarcarFinalizada(respuesta->nodo->jobActivo->transformacion);
		}
	}
	if(tareaEsReduccionLocal(respuesta->tareaEjecutada)){
		if(respuesta->estadoEjecucion == EJECUCION_OK){
			tareaMarcarFinalizada(respuesta->nodo->jobActivo->reduccion_local);
		}
	}
	if(tareaEsReduccionGlobal(respuesta->tareaEjecutada)){
		if(respuesta->estadoEjecucion == EJECUCION_OK){
			tareaMarcarFinalizada(respuesta->nodo->jobActivo->reduccion_global);
		}
	}*/
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
	worker->disponibilidad = base + PWL(worker);
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
