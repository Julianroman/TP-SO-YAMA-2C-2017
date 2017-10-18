/*
 * res_SOLICITUD_ALMACENAMIENTO.c
 */
#include <stdio.h>
#include <commons/collections/dictionary.h>
#include <utilidades/protocol/senders.h>

#include "../../serverYAMA/responses/responses.h"

YAMA_STATUS res_SOLICITUD_ALMACENAMIENTO  (int socket,void*  data, t_dictionary* diccionario){
	payload_SOLICITUD_ALMACENAMIENTO * payload;
	payload = data;
	printf("SOLICITUD_ALMACENAMIENTO recibida\n");

	//DUMMIE WORKERS
	send_INFO_ALMACENAMIENTO(socket,5555,"192.168.1.11","/tmp/Master1-tempredGlob44");
	send_FIN_LISTA(socket);

	// IMPORTANTE! HAY UN LEAK DE MEMORIA
	// LA FUNCION DE RECOLECCION ESTA EN DESARROLLO
	return EXITO;
};
