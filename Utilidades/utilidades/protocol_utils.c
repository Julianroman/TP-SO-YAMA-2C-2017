/*
 * protocol_utils.c
 *
 *  Created on: 17/9/2017
 *      Author: utnso
 */

#include <stdlib.h>
#include "protocolo.h"
#include "protocol_utils.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>


// Cantidad de elementos de un array x
#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))

t_Mensaje recibir_header(int socket){
	t_Mensaje header;
	recv(socket,&header,sizeof header, 0);
	return header;
};

void* buscar_respuesta(t_Mensaje tipoDeMensaje, t_Respuesta* tablaDeAcciones){
	int i, n = NELEMS(tablaDeAcciones);
	for(i=0; i<n; i++){
		if(tipoDeMensaje == tablaDeAcciones[i].estimulo){
			return tablaDeAcciones[i].accion;
		}
	}
	return NULL;
}

void responder_solicitud(t_Mensaje tipoDeMensaje, t_Respuesta* tablaDeAcciones){
	void (*accion) ();
	if((accion = buscar_respuesta(tipoDeMensaje,tablaDeAcciones))!=NULL){
		// ejecuto la interfaz del comando
		accion();
	}else{
		// Sino muestro un error
		printf("Mensaje desconocido\n");
	}
};

