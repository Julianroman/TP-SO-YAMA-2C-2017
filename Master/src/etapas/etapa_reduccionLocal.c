/*
 * etapa_reduccionLocal.c
 */
#include <stdio.h>
#include <unistd.h>
#include <utilidades/protocol/senders.h>
#include <utilidades/protocol/types.h>
#include <utilidades/protocol/receive.h>
#include <utilidades/socket_utils.h>
#include <commons/log.h>
#include <commons/collections/queue.h>
#include <pthread.h>
#include <semaphore.h>
#include "etapas.h"

extern sem_t threadManager;
extern t_log* logger;

void* rutina_reduccionLocal(void* args);
typedef struct{
	t_log* logger;
	payload_INFO_REDUCCIONLOCAL* payload;
}DATA_REDUCCIONLOCAL;

MASTER_STATUS etapa_reduccionLocal (int socketYAMA){
	log_trace(logger, "Iniciando etapa de reduccion local...");

	t_queue * colaDeHilos = queue_create();
	HEADER_T header;
	void* data;
	pthread_t tid;
	pthread_attr_t attr;
	DATA_REDUCCIONLOCAL data_reduccion;

	send_SOLICITUD_REDUCCIONLOCAL(socketYAMA);
	data = receive(socketYAMA,&header);

	if (header == FIN_COMUNICACION){ /*Si header es FIN_COMUNICACION es porque se cerro la conexion*/ }

	// Recibir todas las instrucciones
	// Hasta que termine la lista
	while(header != FIN_LISTA){
		if (header == INFO_REDUCCIONLOCAL){
			data_reduccion.logger = logger;
			data_reduccion.payload = data;
			// Inicializo atributos del thread
			pthread_attr_init(&attr);
			// Creo el hilo
			pthread_create(&tid, &attr, rutina_reduccionLocal, &data_reduccion);
			// Lo agrego a la cola de hilos
			queue_push(colaDeHilos,&tid);
		}
		// IMPORTANTE! HAY UN LEAK DE MEMORIA
		// LA FUNCION DE RECOLECCION ESTA EN DESARROLLO
		data = receive(socketYAMA,&header);
		if (header == FIN_COMUNICACION){ break;/*Si header es FIN_COMUNICACION es porque se cerro la conexion */}
	}

	int i = 0;
	for(i ; i < queue_size(colaDeHilos) ; i++ ){
		sem_wait(&threadManager);
	}

	pthread_t* hiloActual;
	while(!(queue_is_empty(colaDeHilos))){
		hiloActual = queue_pop(colaDeHilos);
		pthread_join(*hiloActual,NULL);
	}
	queue_destroy(colaDeHilos);
	return EXITO;
};

void* rutina_reduccionLocal(void* args){
	HEADER_T header;
	DATA_REDUCCIONLOCAL* data = args;
	t_log* logger = data->logger;
	payload_INFO_REDUCCIONLOCAL* payload = data->payload;

	int socketWorker = crear_conexion(payload->IP_Worker,payload->PUERTO_Worker);
	send_ORDEN_REDUCCIONLOCAL(socketWorker,payload->nombreTemporal_Transformacion,payload->nombreTemporal_ReduccionLocal);

	receive(socketWorker,&header);
	if(header == FIN_LISTA){
		log_info(logger, "Reduccion completada %s ---> %s",payload->nombreTemporal_Transformacion,payload->nombreTemporal_ReduccionLocal);
	}else{
		log_error(logger, "Reduccion interrumpida %s -/-> %s",payload->nombreTemporal_Transformacion,payload->nombreTemporal_ReduccionLocal);
	}
	close(socketWorker);
	sem_post(&threadManager);
	pthread_exit(0);
}
