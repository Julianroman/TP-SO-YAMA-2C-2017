/*
 * etapa_transformacion.c
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

void* rutina_transformacion(void* args);
typedef struct{
	t_log* logger;
	payload_INFO_TRANSFORMACION* payload;
}DATA_TRANSFORMACION;

MASTER_STATUS etapa_transformacion (int socketYAMA, char* yamafs_archivo){
	log_trace(logger, "Iniciando etapa de transformacion...");

	t_queue * colaDeHilos = queue_create();
	HEADER_T header;
	void* data;
	pthread_t tid;
	pthread_attr_t attr;
	DATA_TRANSFORMACION data_transformacion;

	send_SOLICITUD_PROCESAMIENTO(socketYAMA,yamafs_archivo);
	data = receive(socketYAMA,&header);

	if (header == FIN_COMUNICACION){ /*Si header es FIN_COMUNICACION es porque se cerro la conexion*/ }
	// Recibir todas las instrucciones
	// Hasta que termine la lista
	while(header != FIN_LISTA){
		if (header == INFO_TRANSFORMACION){
			data_transformacion.logger = logger;
			data_transformacion.payload = data;
			// Inicializo atributos del thread
			pthread_attr_init(&attr);
			// Creo el hilo
			pthread_create(&tid, &attr, rutina_transformacion, &data_transformacion);
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


void* rutina_transformacion(void* args){
	HEADER_T header;
	DATA_TRANSFORMACION* data = args;
	t_log* logger = data->logger;
	payload_INFO_TRANSFORMACION* payload = data->payload;

	int socketWorker = crear_conexion(payload->IP_Worker,payload->PUERTO_Worker);
	send_ORDEN_TRANSFORMACION(socketWorker,payload->bloque,payload->bytesocupados,payload->nombreArchivoTemporal);

	receive(socketWorker,&header);
	if(header == FIN_LISTA){
		log_info(logger, "Transformacion completada en %s:%d/BLOCK: %d",payload->IP_Worker,payload->PUERTO_Worker,payload->bloque);
	}else{
		log_error(logger, "Transformacion interrumpida en %s:%d/BLOCK: %d",payload->IP_Worker,payload->PUERTO_Worker,payload->bloque);
	}
	close(socketWorker);
	sem_post(&threadManager);
	pthread_exit(0);
}
