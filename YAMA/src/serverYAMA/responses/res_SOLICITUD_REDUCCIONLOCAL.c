/*
 * res_SOLICITUD_REDUCCIONLOCAL.c
 */
#include <stdio.h>
#include <commons/collections/dictionary.h>
#include <utilidades/protocol/senders.h>

#include "../../serverYAMA/responses/responses.h"

YAMA_STATUS res_SOLICITUD_REDUCCIONLOCAL  (int socket,void*  data, t_dictionary* diccionario){
	payload_SOLICITUD_REDUCCIONLOCAL* payload;
	payload = data;
	printf("SOLICITUD_REDUCCIONLOCAL recibida \n");

	//DUMMIE WORKERS

	send_INFO_REDUCCIONLOCAL(socket,9095,"127.0.0.1","/tmp/Master1-temp38","/tmp/Master1-tempredLoc38");
	send_INFO_REDUCCIONLOCAL(socket,9095,"127.0.0.1","/tmp/Master1-temp39","/tmp/Master1-tempredLoc39");
	send_INFO_REDUCCIONLOCAL(socket,9095,"127.0.0.1","/tmp/Master1-temp44","/tmp/Master1-tempredLoc44");
	send_INFO_REDUCCIONLOCAL(socket,9095,"127.0.0.1","/tmp/Master1-temp39","/tmp/Master1-tempredLoc39");
	send_INFO_REDUCCIONLOCAL(socket,9095,"127.0.0.1","/tmp/Master1-temp46","/tmp/Master1-tempredLoc46");

	send_FIN_LISTA(socket);
	puts("Mensajes enviados");

	// IMPORTANTE! HAY UN LEAK DE MEMORIA
	// LA FUNCION DE RECOLECCION ESTA EN DESARROLLO
	return EXITO;
};
