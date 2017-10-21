/*
 * res_SOLICITUD_REDUCCIONGLOBAL.c
 */
#include <stdio.h>
#include <commons/collections/dictionary.h>
#include <utilidades/protocol/senders.h>

#include "../../serverYAMA/responses/responses.h"

YAMA_STATUS res_SOLICITUD_REDUCCIONGLOBAL  (int socket,void*  data, t_dictionary* diccionario){
	payload_SOLICITUD_REDUCCIONGLOBAL * payload;
	payload = data;
	printf("SOLICITUD_REDUCCIONGLOBAL recibida\n");

	//DUMMIE WORKERS

	send_INFO_REDUCCIONGLOBAL(socket,9095,"127.0.0.1","/tmp/Master1-tempredLoc44","/tmp/Master1-tempredGlob44",1);
	send_INFO_REDUCCIONGLOBAL(socket,9095,"127.0.0.1","/tmp/Master1-tempredLoc38"," ",0);
	send_INFO_REDUCCIONGLOBAL(socket,9095,"127.0.0.1","/tmp/Master1-tempredLoc39"," ",0);
	send_INFO_REDUCCIONGLOBAL(socket,9095,"127.0.0.1","/tmp/Master1-tempredLoc40"," ",0);
	send_INFO_REDUCCIONGLOBAL(socket,9095,"127.0.0.1","/tmp/Master1-tempredLoc46"," ",0);
	send_FIN_LISTA(socket);

	// IMPORTANTE! HAY UN LEAK DE MEMORIA
	// LA FUNCION DE RECOLECCION ESTA EN DESARROLLO
	return EXITO;
};
