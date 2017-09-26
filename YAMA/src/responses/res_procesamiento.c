/*
 * res_procesamiento.c
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "../responses.h"

YAMA_STATUS res_procesamiento(int socket){
	printf("Procesamiento iniciado ");
	payload_solicitud_procesamiento payload;
	recv(socket, &payload, sizeof payload, 0);
	printf("para archivo %s \n",payload.nombreArchivo); // Esto no funciona evidentemente, pero se solucionara cuando se implemente serializacion
														// De cualquier modo, se recibe correctamente, solo que no se interpreta bien
	return EXITO;
};

