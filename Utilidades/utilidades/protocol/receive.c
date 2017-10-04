/*
 * receive.c
 * Generado automaticamente por ProtocolMaker
*/

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "types.h"
#include "unpackers.h"

void* receive(int socket,HEADER_T* cabecera){
    void* payload;
    HEADER_T header;
    int status = recv(socket,&header,sizeof(HEADER_T),0);
    if(!status) return NULL;
    switch(header){
        case SOLICITUDPROCESAMIENTO:
        payload = unpack_SOLICITUDPROCESAMIENTO(socket);
        break;
    }
    (*cabecera) = header;
    return payload;
}
