/*
 * solicitud_procesamiento.c
 *
 *  Created on: 17/9/2017
 *      Author: utnso
 */
#include <utilidades/protocolo.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

void solicitud_procesamiento(int socket, char* nombreArchivo){
	// Armar mensaje
	t_Mensaje header = SOLICITUDPROCESAMIENTO;
	payload_solicitud_procesamiento payload;
	strcpy(payload.nombreArchivo,nombreArchivo);

	//Enviar mensaje
		// Header
	send(socket, &header , sizeof header , 0);
		//Payload
	send(socket, &payload, sizeof payload, 0);
};
