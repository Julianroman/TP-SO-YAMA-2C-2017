/*
 * enviar_paquete.c
 *
 *  Created on: 2/10/2017
 *      Author: utnso
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>


void enviar_paquete(int socket, char * paquete, int tamanio_paquete) {

	char * buffer = malloc(tamanio_paquete);
	memcpy(buffer, paquete, tamanio_paquete);
	send(socket, buffer, tamanio_paquete, MSG_WAITALL);
	free(buffer);

}
