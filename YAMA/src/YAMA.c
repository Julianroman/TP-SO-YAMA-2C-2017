/*
 ============================================================================
 Name        : YAMA.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <utilidades/Sockets.c>
#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>

#include "serverYAMA/responses/responses.h"
#include "serverYAMA/serverYAMA.h"
#include "Job.h"
#include "YAMA.h"


t_log* logs;

t_yama* leerConfiguracion(){
	char* path = "/home/utnso/workspace/tp-2017-2c-Grupo-1---K3525/YAMA/src/yama-config.cfg";
	t_config* archivo_configuracion = config_create(path);
	t_yama *configYAMA = malloc(sizeof(t_yama));
	configYAMA->puertoFs = config_get_int_value(archivo_configuracion, "FS_PUERTO");
	printf("El puerto FS es: %i \n", configYAMA->puertoFs);
	configYAMA->ipFs = config_get_string_value(archivo_configuracion, "FS_IP");
	printf("La IP FS es: %s \n", configYAMA->ipFs);
	configYAMA->retardoPlanificacion = config_get_int_value(archivo_configuracion, "RETARDO_PLANIFICACION");
	printf("El retardo de la Planificacion es: %i \n", configYAMA->retardoPlanificacion);
	configYAMA->algoritmoBalanceo = config_get_string_value(archivo_configuracion, "ALGORITMO_BALANCEO");
	printf("El Algoritmo de Balanceo es: %s \n", configYAMA->algoritmoBalanceo);
	configYAMA->ipYAMA = config_get_int_value(archivo_configuracion, "YAMA_IP");
	printf("YAMA se conectara la IP: %d \n", configYAMA->ipYAMA);
	configYAMA->puertoYAMA = config_get_int_value(archivo_configuracion, "YAMA_PUERTO");
	printf("YAMA se conectara en el puerto: %d \n", configYAMA->puertoYAMA);


	config_destroy(archivo_configuracion);
	return configYAMA;
}

int main(void) {
	puts("Comienza el proceso YAMA");

	logs = log_create("yama.log", "YAMA", false, LOG_LEVEL_TRACE);

	log_trace(logs, "Leyendo configuracion");
	t_yama configYAMA = leerConfiguracion();
	log_trace(logs, "Configuracion leida");

	//cliente(ipFs, puertoFs, id);
	init_serverYAMA(configYAMA->puertoYAMA);

	return EXIT_SUCCESS;
}

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


