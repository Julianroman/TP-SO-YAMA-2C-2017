/*
 * res_SOLICITUD_PROCESAMIENTO.c
 */
#include <stdio.h>
#include <commons/collections/dictionary.h>
#include <utilidades/protocol/senders.h>

#include "../../serverYAMA/responses/responses.h"
#include "Job.h"
#include "Tarea.h"

YAMA_STATUS res_SOLICITUD_PROCESAMIENTO  (int socket,void*  data, t_dictionary* diccionario){
	payload_SOLICITUD_PROCESAMIENTO* payload = data;
	printf("SOLICITUD_PROCESAMIENTO recibida\n");
	printf("Archivo: %s, enviando informacion sobre workers\n",payload->nombreArchivo);



	//DUMMIE WORKERS
	send_INFO_TRANSFORMACION(socket,9095,"127.0.0.1",38,10180,"/tmp/Master1-temp38");
	send_INFO_TRANSFORMACION(socket,9095,"127.0.0.1",39,10180,"/tmp/Master1-temp39");
	send_INFO_TRANSFORMACION(socket,9095,"127.0.0.1",44,10180,"/tmp/Master1-temp44");
	send_FIN_LISTA(socket);

	// IMPORTANTE! HAY UN LEAK DE MEMORIA
	// LA FUNCION DE RECOLECCION ESTA EN DESARROLLO
	return EXITO;
};
