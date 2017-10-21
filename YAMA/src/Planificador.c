/*
 * Planificador.c
 *
 *  Created on: 20/10/2017
 *      Author: utnso
 */

#include "Planificador.h"

typedef struct {
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

/*int main(void) { 				era para probar
	puts("hola");
	worker1 = malloc(sizeof(t_nodo));
	worker2 = malloc(sizeof(t_nodo));
	worker1->carga = 0;
	worker1->bloquesMios = list_create();
	worker2->bloquesMios = list_create();
	list_add(worker1->bloquesMios, "1");
	list_add(worker1->bloquesMios, "2");
	worker2->carga = 0;
	list_add(worker2->bloquesMios, "0");
	list_add(worker2->bloquesMios, "3");
	listaWorkers = list_create();
	list_add(listaWorkers, worker1);
	list_add(listaWorkers, worker2);
 	planificacionClock(listaWorkers);

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
	char* bloquesTotales = {"0","1","2","3"};
	valor = listaNodos->head;
	for(i = 0; i < sizeof(bloquesTotales);i++){
		while(1){
			workerActual = valor->data;
			if(workerActual->carga > 0){
				////// hacer esta funcion y descomentar /////////
			//	if(existeEn(workerActual->bloquesMios,bloquesTotales[i]) != NULL){
				//////////////////////////////////
					list_add(workerActual->bloquesAdquiridos,bloquesTotales[i]);
					workerActual->carga -= 1;
					if(valor->next){
						valor = valor->next;
						verificador = 0;
					}else{
						verificador = 1;
					}
					if(verificador == 1)
						valor = listaNodos->head;
				//}
				break;
			}else{
					workerActual->carga = base;

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
	}
}

int existeEn(t_list* lista , void* dato){

	return 1;
}
/*t_nodo* workerConMenorCarga(t_list * listaWorker){
	t_nodo* workerMin = malloc(sizeof(t_nodo));
	workerMin = listaWorker->head->data;
	t_nodo* workerActual = malloc(sizeof(t_nodo));
	t_link_element* valor = malloc(sizeof(t_link_element));
	valor = listaWorker->head;
	int verificador = 0;
	while(verificador == 0){
		workerActual = valor->data;
		if(workerActual->carga > workerMin->carga){
			workerMin = workerActual;
		}
		if(valor->next){
			valor = valor->next;
		}else{
			verificador = 1;
		}
	}
		return workerMin;
}*/

