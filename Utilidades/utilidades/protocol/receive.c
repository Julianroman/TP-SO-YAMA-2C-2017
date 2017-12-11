
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
    if(!status){
         (*cabecera) = FIN_COMUNICACION;
         return NULL;
    }
    switch(header){
        case SOLICITUD_JOB:
        payload = unpack_SOLICITUD_JOB(socket);
        break;
        case ORDEN_TRANSFORMACION:
        payload = unpack_ORDEN_TRANSFORMACION(socket);
        break;
        case ORDEN_REDUCCIONLOCAL:
        payload = unpack_ORDEN_REDUCCIONLOCAL(socket);
        break;
        case ORDEN_REDUCCIONGLOBAL:
        payload = unpack_ORDEN_REDUCCIONGLOBAL(socket);
        break;
        case ORDEN_ALMACENAMIENTO:
        payload = unpack_ORDEN_ALMACENAMIENTO(socket);
        break;
        case INFO_TRANSFORMACION:
        payload = unpack_INFO_TRANSFORMACION(socket);
        break;
        case INFO_REDUCCIONLOCAL:
        payload = unpack_INFO_REDUCCIONLOCAL(socket);
        break;
        case INFO_REDUCCIONGLOBAL:
        payload = unpack_INFO_REDUCCIONGLOBAL(socket);
        break;
        case INFO_ALMACENAMIENTO:
        payload = unpack_INFO_ALMACENAMIENTO(socket);
        break;
        case PETICION_NODO:
        payload = unpack_PETICION_NODO(socket);
        break;
        case NODO:
        payload = unpack_NODO(socket);
        break;
        case FIN_LISTA:
        /* Carece de Payload */
        break;
        case FIN_COMUNICACION:
        /* Carece de Payload */
        break;
        case ARCHIVO:
        payload = unpack_ARCHIVO(socket);
        break;
        case BLOQUE:
        payload = unpack_BLOQUE(socket);
        break;
        case PRESENTACION_DATANODE:
        payload = unpack_PRESENTACION_DATANODE(socket);
        break;
        case JOB:
        payload = unpack_JOB(socket);
        break;
        case RESPUESTA_MASTER:
        payload = unpack_RESPUESTA_MASTER(socket);
        break;
        case EXITO_OPERACION:
        /* Carece de Payload */
        break;
        case FRACASO_OPERACION:
        /* Carece de Payload */
        break;
        case SCRIPT:
        payload = unpack_SCRIPT(socket);
        break;
        case PETICION_BLOQUE:
        payload = unpack_PETICION_BLOQUE(socket);
        break;
        case UBICACION_BLOQUE:
        payload = unpack_UBICACION_BLOQUE(socket);
        break;
        case TEMPORAL:
        payload = unpack_TEMPORAL(socket);
        break;
        case PETICION_TEMPORAL:
        payload = unpack_PETICION_TEMPORAL(socket);
        break;
        case RECHAZO_CONEXION:
        /* Carece de Payload */
        break;
        case ALMACENAR_ARCHIVO:
        payload = unpack_ALMACENAR_ARCHIVO(socket);
        break;
    }
    (*cabecera) = header;
    return payload;
}
