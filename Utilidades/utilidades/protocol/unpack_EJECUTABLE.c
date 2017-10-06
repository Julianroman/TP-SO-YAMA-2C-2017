/*
 * unpack_EJECUTABLE.c
 *
 *  Created on: 5/10/2017
 *      Author: utnso
 */

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "types.h"

void* unpack_EJECUTABLE(int socket){
    payload_EJECUTABLE *payload= malloc(sizeof(payload_EJECUTABLE));

    recv(socket,&(payload->tamanio_archivo),sizeof(uint16_t),0);
    uint16_t  tamanio_nombreArchivo = payload->tamanio_archivo;

    char* nombreArchivo = malloc(tamanio_nombreArchivo);
    recv(socket,nombreArchivo,tamanio_nombreArchivo,0);
    payload->archivo = nombreArchivo;

    return (void*)payload;
};
