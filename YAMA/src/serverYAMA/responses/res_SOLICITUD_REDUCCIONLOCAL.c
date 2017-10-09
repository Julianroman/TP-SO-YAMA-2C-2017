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
	/*
	send_INFO_TRANSFORMACION(socket,5000,"192.168.1.10",38,10180,"/tmp/Master1-temp38");
	send_INFO_TRANSFORMACION(socket,5000,"192.168.1.10",39,10180,"/tmp/Master1-temp39");
	send_INFO_TRANSFORMACION(socket,5555,"192.168.1.11",44,10180,"/tmp/Master1-temp44");
	send_INFO_TRANSFORMACION(socket,5555,"192.168.1.11",39,10180,"/tmp/Master1-temp39");
	send_INFO_TRANSFORMACION(socket,5555,"192.168.1.11",46,10180,"/tmp/Master1-temp46");
	 */
	send_FIN_LISTA(socket);

	// IMPORTANTE! HAY UN LEAK DE MEMORIA
	// LA FUNCION DE RECOLECCION ESTA EN DESARROLLO
	return EXITO;
};
