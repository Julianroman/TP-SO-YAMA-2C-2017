/*
 * send_EJECUTABLE.c
 *
 *  Created on: 5/10/2017
 *      Author: utnso
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "types.h"
#include "enviar_paquete.h"


void send_EJECUTABLE(int socket , int ejecutable_fd){
	//
	struct stat buffer;
	int status = fstat(ejecutable_fd,&buffer);
	if(!status){
		puts("No se pudieron reconocer las estadisticas del ejecutable");
	}
	HEADER_T header = EJECUTABLE;
	uint32_t size = buffer.st_size;

	char* paquete = malloc(sizeof(HEADER_T)+sizeof(uint32_t)+size);
	int offset = 0;
	memcpy(paquete+offset,&header,sizeof(HEADER_T));
	offset += sizeof(HEADER_T);
	memcpy(paquete+offset,&size,sizeof(uint32_t));
	offset += sizeof(uint32_t);
	read(ejecutable_fd,paquete+offset,size);
	enviar_paquete(socket,paquete,size);
	free(paquete);

};
