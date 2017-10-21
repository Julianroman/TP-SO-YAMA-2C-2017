/*
 * Planificador.c
 *
 *  Created on: 20/10/2017
 *      Author: utnso
 */

#include "Planificador.h"
#include "Job.h"
#include "YAMA.h"
#include "Tarea.h"


void inicializarPlanificador(){
	listaNodos = list_create();
	//Habria que cargar la lista de nodos con su carga y disponibilidad
	listaRespuestasMaster = list_create();
	int idUltimoJobCreado = 0;
	int idUltimoJobPlanificado = 0;
	int idUltimaTarea = 0;
	diccionarioJobs = dictionary_create();
	bloques_ejecutados = dictionary_create();
	t_job* job = newJob();
	agregarJob(job);
}

void agregarJob(t_job* job){
	idUltimoJobCreado++;
	job->id = idUltimoJobCreado;
	char* keyJob = string_itoa(idUltimoJobCreado);

	dictionary_put(diccionarioJobs, keyJob, job);
	//Actualizar tabla de estados con el job creado
}

void iniciarPlanificacion(){
	inicializarPlanificador();
	t_list* nodosDisponibles = obtenerNodosParaPlanificacion(); //Funcion a desarrollar conjuntamente con FS
	planificacionClock(nodosDisponibles); //Deberia ser WClock
	respuestaInfoMaster* respuesta = obtenerSiguienteInfoMaster(); //Master me encola todas las respuestas que tuvo de los workers - Devuelve el worker que necesita siguiente instruccion
	while(!todosLosNodosTerminaronReduccionLocal(nodosDisponibles)){
		realizarSiguienteInstruccion(respuesta);
	}
	t_nodo* encargado = elegirEncargadoReduccionGlobal(nodosDisponibles);
	realizarReduccionGlobal(encargado);
	finalizar(); // Como debe finalizar todo?
}

int todosLosNodosTerminaronReduccionLocal(t_list* nodosDisponibles){
	int nodoTerminoReduccionLocal(t_nodo* nodo){
		return tareaEstaFinalizada(nodo->jobActivo->reduccion_local);
	}
	return list_all_satisfy(nodosDisponibles, (void*) nodoTerminoReduccionLocal);
}

void realizarSiguienteinstruccion(respuestaInfoMaster* respuesta){
	if(respuesta->estadoEjecucion == EJECUCION_ERROR){
		if(respuesta->tareaEjecutada == REDUCCION_LOCAL || respuesta->tareaEjecutada == REDUCCION_GLOBAL){
			abortarJob(respuesta->jobEjecutado);
		}
		else{
			planificarTransformacion(respuesta->nodo); // Si el error se da en la tarea de trasnformacion como se vuelve a planificar?
		}
	}
	else if(respuesta->estadoEjecucion == EJECUCION_OK){
		realizarSiguienteTarea(respuesta->nodo);
	}
}

respuestaInfoMaster* obtenerSiguienteInfoMaster(){
	respuestaInfoMaster* respuesta = list_remove(listaRespuestasMaster, 0); // Lo retorna y después lo remueve de la lista, así siempre si saco el primero de la lista es una instruccion que nunca saqué
	actualizarEstados(respuesta);
	return respuesta->nodo;
}

void actualizarEstados(respuestaInfoMaster* respuesta){
	actualizarTablaestados(respuesta);
	actualizarLog(respuesta);
	actualizarEstadosNodo(respuesta);
}

void actualizarTablaEstados(respuestaInfoMaster* respuesta){
	t_tablaEstados tablaEstados;
	tablaEstados->job = respuesta->jobEjecutado;
	tablaEstados->master = respuesta->master;
	tablaEstados->nodo = respuesta->nodo;
	//tablaEstados->bloque = Habría que sacar la info del bloque
	tablaEstados->etapa = respuesta->tareaEjecutada;
	//tablaEstados->archivoTemporal = idem bloque
	tablaEstados->estado = respuesta->estadoEjecucion;
}

void actualizarEstadosNodo(respuestaInfoMaster* respuesta){
	switch(respuesta->tareaEjecutada){
	case "TRANSFORMACION":
		if(respuesta->estadoEjecucion == "EJECUCION_OK"){
			tareaMarcarFinalizada(respuesta->nodo->jobActivo->transformacion);
		}
	break;
	case "REDUCCION_GLOBAL":
		if(respuesta->estadoEjecucion == "EJECUCION_OK"){
			tareaMarcarFinalizada(respuesta->nodo->jobActivo->reduccion_local);
		}
	break;
	case "REDUCCION_GLOBAL":
		if(respuesta->estadoEjecucion == "EJECUCION_OK"){
			tareaMarcarFinalizada(respuesta->nodo->jobActivo->reduccion_global);
		}
	break;
	}
}

/*typedef struct {
	char *nombre;
	char *ip;
	char *puerto;
	int carga;
	t_list * bloquesMios;
	t_list * bloquesAdquiridos;
} t_nodo;

t_list* listaWorkers;
t_nodo* worker1;
t_nodo* worker2;
t_nodo* worker3;

int main(void) {
	iniciarPlanificador();
	worker1 = malloc(sizeof(t_nodo));
	worker2 = malloc(sizeof(t_nodo));
	worker3 = malloc(sizeof(t_nodo));
	worker1->disponibilidad = 1;
	worker1->cantTareasHistoricas = 1;
	worker1->bloques = list_create();
	worker2->bloques = list_create();
	worker3->bloques = list_create();
	list_add(worker1->bloques, "1");
	list_add(worker1->bloques, "2");
	list_add(worker1->bloques, "0");
	worker2->disponibilidad = 0;
	worker2->cantTareasHistoricas = 2;
	list_add(worker2->bloques, "0");
	list_add(worker2->bloques, "3");
	worker3->disponibilidad = 1;
	worker3->cantTareasHistoricas = 3;
	list_add(worker3->bloques, "1");
	list_add(worker3->bloques, "3");
	list_add(listaNodos, worker1);
	list_add(listaNodos, worker2);
	list_add(listaNodos, worker3);
	nodoConMayorDisponibilidad();
 	planificacionClock(listaNodos);

	return EXIT_SUCCESS;
}*/

void planificacionClock(t_list* listaNodos){//Esta seria la lista o diccionario de workers
	int base = 2;
	t_nodo* workerMin = malloc(sizeof(t_nodo));
	workerMin = listaNodos->head->data;
	t_nodo* workerActual = malloc(sizeof(t_nodo));
	t_link_element* valor = malloc(sizeof(t_link_element));
	int verificador = 0;
	int i;
	char *bloquesTotales[4] = {"0","1","2","3"};
	valor = listaNodos->head;
	for(i = 0; i < sizeof(&bloquesTotales);i++){
		while(1){
			workerActual = valor->data;
			if(existeEn(workerActual->bloques, bloquesTotales[i]) == 0){
				if(workerActual->disponibilidad > 0){
					dictionary_put(bloques_ejecutados, bloquesTotales[i], workerActual);
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
			 return strcmp(d, dato);
	}
	return list_find(lista, (void*) existeBloque);
}

void nodoConMayorDisponibilidad(){ // ordena la lista de nodos segun la disponibilidad
	t_nodo* worker;
	int mayorDisponibilidad(t_nodo* nodo1, t_nodo* nodo2){
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

int disponibilidad(t_nodo* worker){
	return worker->disponibilidad;
}

int tareasHistoricas(t_nodo* worker){
	return worker->cantTareasHistoricas;
}

t_nodo* buscarNodo(t_list* listaNodos, int numNodo){
	int nodoConNombre(t_nodo* nodo){
		return nodo->numero == numNodo;
	}
	return list_find(listaNodos, (void*) nodoConNombre);
}

int estaActivo(t_nodo* worker){
	return worker->activo == 1;
}

/*void calcularDisponibilidadWorker(t_nodo* worker){
	worker->disponibilidad = getDisponibilidadBase() + calcularPWL(worker); //Esto es la base de como trabaja el algoritmo wClock
}*/
