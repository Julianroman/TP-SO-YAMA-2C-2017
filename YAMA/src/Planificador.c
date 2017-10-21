/*
 * Planificador.c
 *
 *  Created on: 20/10/2017
 *      Author: utnso
 */

#include "Planificador.h"

/*
void planificacionClock(t_list* listaWorker){//Esta seria la lista o diccionario de workers
	int base;
	int i;
	t_nodo* workerInicial = workerConMenorCarga(listaWorker); //agarro el primer worker para comparar
	t_nodo* workerActual = listaWorker->head;
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
				workerActual = listaWorker->head;
		}
		workerActual = workerActual->next;
	}
}

t_nodo* workerConMenorCarga(t_list * listaWorker) {
	int i = 0;
	t_nodo* workerMin = listaWorker->head;
	t_nodo* workerActual= listaWorker->head;
	while(i <= list_size(listaWorker)){
		if(workerActual->carga < workerMin->carga){
			workerMin = workerActual;
		}
		i++;
		workerActual = list_get(listaWorker, i);
	}
		return workerMin;
}*/
