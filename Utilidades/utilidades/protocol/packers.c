/*
 * packers.c
 * Generado automaticamente por ProtocolMaker
*/

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "types.h"

char* pack_SOLICITUDPROCESAMIENTO(payload_SOLICITUDPROCESAMIENTO payload,int* tamanio_paquete){
    int tamanio_total = sizeof(HEADER_T) + sizeof(uint16_t) + (payload.tamanio_nombreArchivo);
    char* paquete = malloc(tamanio_total);

    int offset = 0;
    int tamanio_envio;
    HEADER_T cabecera = SOLICITUDPROCESAMIENTO;
    tamanio_envio = (sizeof(HEADER_T));
    memcpy(paquete+offset,&cabecera,tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(uint16_t);
    memcpy(paquete+offset,&(payload.tamanio_nombreArchivo),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = (payload.tamanio_nombreArchivo);
    memcpy(paquete+offset,payload.nombreArchivo,tamanio_envio);
    offset += tamanio_envio;

    (* tamanio_paquete) = tamanio_total;
    return paquete;
};

