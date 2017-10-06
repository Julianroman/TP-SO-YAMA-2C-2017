/*
 * unpackers.c
 * Generado automaticamente por ProtoCool
*/

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "types.h"

void* unpack_SOLICITUD_PROCESAMIENTO(int socket){
    payload_SOLICITUD_PROCESAMIENTO *payload= malloc(sizeof(payload_SOLICITUD_PROCESAMIENTO));

    recv(socket,&(payload->tamanio_nombreArchivo),sizeof(uint16_t),0);
    uint16_t  tamanio_nombreArchivo = payload->tamanio_nombreArchivo;

    char* nombreArchivo = malloc(tamanio_nombreArchivo);
    recv(socket,nombreArchivo,tamanio_nombreArchivo,0);
    payload->nombreArchivo = nombreArchivo;

    return (void*)payload;
};

void* unpack_ORDEN_TRANSFORMACION(int socket){
    payload_ORDEN_TRANSFORMACION *payload= malloc(sizeof(payload_ORDEN_TRANSFORMACION));

    recv(socket,&(payload->bloque),sizeof(uint16_t),0);

    recv(socket,&(payload->bytesocupados),sizeof(uint16_t),0);

    recv(socket,&(payload->tamanio_nombreArchivoTemporal),sizeof(uint16_t),0);
    uint16_t  tamanio_nombreArchivoTemporal = payload->tamanio_nombreArchivoTemporal;

    char* nombreArchivoTemporal = malloc(tamanio_nombreArchivoTemporal);
    recv(socket,nombreArchivoTemporal,tamanio_nombreArchivoTemporal,0);
    payload->nombreArchivoTemporal = nombreArchivoTemporal;

    return (void*)payload;
};

void* unpack_ORDEN_REDUCCIONLOCAL(int socket){
    payload_ORDEN_REDUCCIONLOCAL *payload= malloc(sizeof(payload_ORDEN_REDUCCIONLOCAL));

    recv(socket,&(payload->tamanio_nombreTemporal_Transformacion),sizeof(uint16_t),0);
    uint16_t  tamanio_nombreTemporal_Transformacion = payload->tamanio_nombreTemporal_Transformacion;

    char* nombreTemporal_Transformacion = malloc(tamanio_nombreTemporal_Transformacion);
    recv(socket,nombreTemporal_Transformacion,tamanio_nombreTemporal_Transformacion,0);
    payload->nombreTemporal_Transformacion = nombreTemporal_Transformacion;

    recv(socket,&(payload->tamanio_nombreTemporal_ReduccionLocal),sizeof(uint16_t),0);
    uint16_t  tamanio_nombreTemporal_ReduccionLocal = payload->tamanio_nombreTemporal_ReduccionLocal;

    char* nombreTemporal_ReduccionLocal = malloc(tamanio_nombreTemporal_ReduccionLocal);
    recv(socket,nombreTemporal_ReduccionLocal,tamanio_nombreTemporal_ReduccionLocal,0);
    payload->nombreTemporal_ReduccionLocal = nombreTemporal_ReduccionLocal;

    return (void*)payload;
};

void* unpack_ORDEN_REDUCCIONGLOBAL(int socket){
    payload_ORDEN_REDUCCIONGLOBAL *payload= malloc(sizeof(payload_ORDEN_REDUCCIONGLOBAL));

    recv(socket,&(payload->PUERTO_Nodo),sizeof(uint16_t),0);

    recv(socket,&(payload->tamanio_IP_Nodo),sizeof(uint16_t),0);
    uint16_t  tamanio_IP_Nodo = payload->tamanio_IP_Nodo;

    char* IP_Nodo = malloc(tamanio_IP_Nodo);
    recv(socket,IP_Nodo,tamanio_IP_Nodo,0);
    payload->IP_Nodo = IP_Nodo;

    recv(socket,&(payload->tamanio_nombreTemporal_ReduccionLocal),sizeof(uint16_t),0);
    uint16_t  tamanio_nombreTemporal_ReduccionLocal = payload->tamanio_nombreTemporal_ReduccionLocal;

    char* nombreTemporal_ReduccionLocal = malloc(tamanio_nombreTemporal_ReduccionLocal);
    recv(socket,nombreTemporal_ReduccionLocal,tamanio_nombreTemporal_ReduccionLocal,0);
    payload->nombreTemporal_ReduccionLocal = nombreTemporal_ReduccionLocal;

    recv(socket,&(payload->tamanio_nombreTemporal_ReduccionGlobal),sizeof(uint16_t),0);
    uint16_t  tamanio_nombreTemporal_ReduccionGlobal = payload->tamanio_nombreTemporal_ReduccionGlobal;

    char* nombreTemporal_ReduccionGlobal = malloc(tamanio_nombreTemporal_ReduccionGlobal);
    recv(socket,nombreTemporal_ReduccionGlobal,tamanio_nombreTemporal_ReduccionGlobal,0);
    payload->nombreTemporal_ReduccionGlobal = nombreTemporal_ReduccionGlobal;

    recv(socket,&(payload->encargado),sizeof(uint16_t),0);

    return (void*)payload;
};

void* unpack_ORDEN_ALMACENAMIENTO(int socket){
    payload_ORDEN_ALMACENAMIENTO *payload= malloc(sizeof(payload_ORDEN_ALMACENAMIENTO));

    recv(socket,&(payload->tamanio_nombreTemporal_ReduccionGlobal),sizeof(uint16_t),0);
    uint16_t  tamanio_nombreTemporal_ReduccionGlobal = payload->tamanio_nombreTemporal_ReduccionGlobal;

    char* nombreTemporal_ReduccionGlobal = malloc(tamanio_nombreTemporal_ReduccionGlobal);
    recv(socket,nombreTemporal_ReduccionGlobal,tamanio_nombreTemporal_ReduccionGlobal,0);
    payload->nombreTemporal_ReduccionGlobal = nombreTemporal_ReduccionGlobal;

    return (void*)payload;
};

void* unpack_INFO_TRANSFORMACION(int socket){
    payload_INFO_TRANSFORMACION *payload= malloc(sizeof(payload_INFO_TRANSFORMACION));

    recv(socket,&(payload->PUERTO_Worker),sizeof(uint16_t),0);

    recv(socket,&(payload->tamanio_IP_Worker),sizeof(uint16_t),0);
    uint16_t  tamanio_IP_Worker = payload->tamanio_IP_Worker;

    char* IP_Worker = malloc(tamanio_IP_Worker);
    recv(socket,IP_Worker,tamanio_IP_Worker,0);
    payload->IP_Worker = IP_Worker;

    recv(socket,&(payload->bloque),sizeof(uint16_t),0);

    recv(socket,&(payload->bytesocupados),sizeof(uint16_t),0);

    recv(socket,&(payload->tamanio_nombreArchivoTemporal),sizeof(uint16_t),0);
    uint16_t  tamanio_nombreArchivoTemporal = payload->tamanio_nombreArchivoTemporal;

    char* nombreArchivoTemporal = malloc(tamanio_nombreArchivoTemporal);
    recv(socket,nombreArchivoTemporal,tamanio_nombreArchivoTemporal,0);
    payload->nombreArchivoTemporal = nombreArchivoTemporal;

    return (void*)payload;
};

void* unpack_INFO_REDUCCIONLOCAL(int socket){
    payload_INFO_REDUCCIONLOCAL *payload= malloc(sizeof(payload_INFO_REDUCCIONLOCAL));

    recv(socket,&(payload->PUERTO_Worker),sizeof(uint16_t),0);

    recv(socket,&(payload->tamanio_IP_Worker),sizeof(uint16_t),0);
    uint16_t  tamanio_IP_Worker = payload->tamanio_IP_Worker;

    char* IP_Worker = malloc(tamanio_IP_Worker);
    recv(socket,IP_Worker,tamanio_IP_Worker,0);
    payload->IP_Worker = IP_Worker;

    recv(socket,&(payload->tamanio_nombreTemporal_Transformacion),sizeof(uint16_t),0);
    uint16_t  tamanio_nombreTemporal_Transformacion = payload->tamanio_nombreTemporal_Transformacion;

    char* nombreTemporal_Transformacion = malloc(tamanio_nombreTemporal_Transformacion);
    recv(socket,nombreTemporal_Transformacion,tamanio_nombreTemporal_Transformacion,0);
    payload->nombreTemporal_Transformacion = nombreTemporal_Transformacion;

    recv(socket,&(payload->tamanio_nombreTemporal_ReduccionLocal),sizeof(uint16_t),0);
    uint16_t  tamanio_nombreTemporal_ReduccionLocal = payload->tamanio_nombreTemporal_ReduccionLocal;

    char* nombreTemporal_ReduccionLocal = malloc(tamanio_nombreTemporal_ReduccionLocal);
    recv(socket,nombreTemporal_ReduccionLocal,tamanio_nombreTemporal_ReduccionLocal,0);
    payload->nombreTemporal_ReduccionLocal = nombreTemporal_ReduccionLocal;

    return (void*)payload;
};

void* unpack_INFO_REDUCCIONGLOBAL(int socket){
    payload_INFO_REDUCCIONGLOBAL *payload= malloc(sizeof(payload_INFO_REDUCCIONGLOBAL));

    recv(socket,&(payload->PUERTO_Worker),sizeof(uint16_t),0);

    recv(socket,&(payload->tamanio_IP_Worker),sizeof(uint16_t),0);
    uint16_t  tamanio_IP_Worker = payload->tamanio_IP_Worker;

    char* IP_Worker = malloc(tamanio_IP_Worker);
    recv(socket,IP_Worker,tamanio_IP_Worker,0);
    payload->IP_Worker = IP_Worker;

    recv(socket,&(payload->tamanio_nombreTemporal_ReduccionLocal),sizeof(uint16_t),0);
    uint16_t  tamanio_nombreTemporal_ReduccionLocal = payload->tamanio_nombreTemporal_ReduccionLocal;

    char* nombreTemporal_ReduccionLocal = malloc(tamanio_nombreTemporal_ReduccionLocal);
    recv(socket,nombreTemporal_ReduccionLocal,tamanio_nombreTemporal_ReduccionLocal,0);
    payload->nombreTemporal_ReduccionLocal = nombreTemporal_ReduccionLocal;

    recv(socket,&(payload->tamanio_nombreTemporal_ReduccionGlobal),sizeof(uint16_t),0);
    uint16_t  tamanio_nombreTemporal_ReduccionGlobal = payload->tamanio_nombreTemporal_ReduccionGlobal;

    char* nombreTemporal_ReduccionGlobal = malloc(tamanio_nombreTemporal_ReduccionGlobal);
    recv(socket,nombreTemporal_ReduccionGlobal,tamanio_nombreTemporal_ReduccionGlobal,0);
    payload->nombreTemporal_ReduccionGlobal = nombreTemporal_ReduccionGlobal;

    recv(socket,&(payload->encargado),sizeof(uint16_t),0);

    return (void*)payload;
};

void* unpack_INFO_ALMACENAMIENTO(int socket){
    payload_INFO_ALMACENAMIENTO *payload= malloc(sizeof(payload_INFO_ALMACENAMIENTO));

    recv(socket,&(payload->PUERTO_Worker),sizeof(uint16_t),0);

    recv(socket,&(payload->tamanio_IP_Worker),sizeof(uint16_t),0);
    uint16_t  tamanio_IP_Worker = payload->tamanio_IP_Worker;

    char* IP_Worker = malloc(tamanio_IP_Worker);
    recv(socket,IP_Worker,tamanio_IP_Worker,0);
    payload->IP_Worker = IP_Worker;

    recv(socket,&(payload->tamanio_nombreTemporal_ReduccionGlobal),sizeof(uint16_t),0);
    uint16_t  tamanio_nombreTemporal_ReduccionGlobal = payload->tamanio_nombreTemporal_ReduccionGlobal;

    char* nombreTemporal_ReduccionGlobal = malloc(tamanio_nombreTemporal_ReduccionGlobal);
    recv(socket,nombreTemporal_ReduccionGlobal,tamanio_nombreTemporal_ReduccionGlobal,0);
    payload->nombreTemporal_ReduccionGlobal = nombreTemporal_ReduccionGlobal;

    return (void*)payload;
};

