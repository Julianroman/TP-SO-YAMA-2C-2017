/*
 * senders.c
 * Generado automaticamente por ProtocolMaker
*/

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "types.h"
#include "packers.h"
#include "enviar_paquete.h"

void send_SOLICITUDPROCESAMIENTO(int socket, char* nombreArchivo){
    payload_SOLICITUDPROCESAMIENTO payload;
    payload.tamanio_nombreArchivo = (strlen(nombreArchivo)+1)*sizeof(char);
    payload.nombreArchivo = nombreArchivo; 

    int tamanio_paquete;
    char* paquete = pack_SOLICITUDPROCESAMIENTO(payload,&tamanio_paquete);
    enviar_paquete(socket,paquete,tamanio_paquete);
    free(paquete);
};

