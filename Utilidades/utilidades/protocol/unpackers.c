/*
 * unpackers.c
 * Generado automaticamente por ProtocolMaker
*/

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "types.h"

void* unpack_SOLICITUDPROCESAMIENTO(int socket){
    payload_SOLICITUDPROCESAMIENTO *payload = malloc(sizeof(payload_SOLICITUDPROCESAMIENTO));

    recv(socket,&(payload->tamanio_nombreArchivo),sizeof(uint16_t),0);
    uint16_t  tamanio_nombreArchivo = payload->tamanio_nombreArchivo;

    char* nombreArchivo = malloc(tamanio_nombreArchivo);
    recv(socket,nombreArchivo,tamanio_nombreArchivo,0);
    payload->nombreArchivo = nombreArchivo;

    return (void*)payload;
};

