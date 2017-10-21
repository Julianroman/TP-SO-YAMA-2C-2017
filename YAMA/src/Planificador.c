/*
 * Planificador.c
 *
 *  Created on: 20/10/2017
 *      Author: utnso
 */

#include "Planificador.h"
#include "Job.h"

static int idUltimoJobCreado;
static int idUltimoJobPlanificado;
static int idUltimaTarea;

void iniciarPlanificador(){
	listaNodos = list_create();
	//Habria que cargar la lista de nodos con su carga y disponibilidad
	int idUltimoJobCreado = 0;
	int idUltimoJobPlanificado = 0;
	int idUltimaTarea = 0;
	diccionarioJobs = dictionary_create();
}

void agregarJob(t_job* job){
	idUltimoJobCreado++;
	job->id = idUltimoJobCreado;
	char* keyJob = string_itoa(idUltimoJobCreado);

	dictionary_put(diccionarioJobs, keyJob, job);
	//Actualizar tabla de estados con el job creado
}

/*void planificacionClock(t_list* listaNodos){//Esta seria la lista o diccionario de workers
	int base;
	int i;
	t_nodo* workerInicial = workerConMenorCarga(listaNodos); //agarro el primer worker para comparar
	t_nodo* workerActual = listaNodos->head;
	char* bloquesTotales;
	for(i = 0; i < sizeof(bloquesTotales);i++){
		while(i != '\0'){
			//auxiliar->head->data te da los datos de la primea posicion de la lista
			//auxiliar->head->next te da el siguiente
			if(existeEn(workerActual,bloquesTotales[i]) && workerActual->carga > 0){
				asignarBloque(workerActual,bloquesTotales[i]);
				workerActual->carga -= 1;
				break;
			}else{
				workerActual->carga = 0;
				//es para saltearse el que tenga disponibilidad 0
				workerActual = workerActual->next;
			}
			workerActual = workerActual->next;
			if(workerActual == NULL)
				//reiniciar el recorrido de la lista
				workerActual = listaNodos->head;
		}
		workerActual = workerActual->next;
	}
}

t_nodo* workerConMenorCarga(t_list * listaNodos) {
	int i = 0;
	t_nodo* workerMin = listaNodos->head;
	t_nodo* workerActual= listaNodos->head;
	while(i <= list_size(listaNodos)){
		if(workerActual->carga < workerMin->carga){
			workerMin = workerActual;
		}
		i++;
		workerActual = list_get(listaNodos, i);
	}
		return workerMin;
}*/
