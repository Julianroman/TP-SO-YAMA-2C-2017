
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "types.h"

void* unpack_SOLICITUD_JOB(int socket){
    payload_SOLICITUD_JOB *payload= malloc(sizeof(payload_SOLICITUD_JOB));

    recv(socket,&(payload->tamanio_nombreArchivo),sizeof(int),0);
    int  tamanio_nombreArchivo = payload->tamanio_nombreArchivo;

    char* nombreArchivo = malloc(tamanio_nombreArchivo);
    recv(socket,nombreArchivo,tamanio_nombreArchivo,0);
    payload->nombreArchivo = nombreArchivo;

    return (void*)payload;
};

void* unpack_ORDEN_TRANSFORMACION(int socket){
    payload_ORDEN_TRANSFORMACION *payload= malloc(sizeof(payload_ORDEN_TRANSFORMACION));

    recv(socket,&(payload->bloque),sizeof(int),0);

    recv(socket,&(payload->bytesocupados),sizeof(int),0);

    recv(socket,&(payload->tamanio_nombreArchivoTemporal),sizeof(int),0);
    int  tamanio_nombreArchivoTemporal = payload->tamanio_nombreArchivoTemporal;

    char* nombreArchivoTemporal = malloc(tamanio_nombreArchivoTemporal);
    recv(socket,nombreArchivoTemporal,tamanio_nombreArchivoTemporal,0);
    payload->nombreArchivoTemporal = nombreArchivoTemporal;

    return (void*)payload;
};

void* unpack_ORDEN_REDUCCIONLOCAL(int socket){
    payload_ORDEN_REDUCCIONLOCAL *payload= malloc(sizeof(payload_ORDEN_REDUCCIONLOCAL));

    recv(socket,&(payload->tamanio_nombreTemporal_Transformacion),sizeof(int),0);
    int  tamanio_nombreTemporal_Transformacion = payload->tamanio_nombreTemporal_Transformacion;

    char* nombreTemporal_Transformacion = malloc(tamanio_nombreTemporal_Transformacion);
    recv(socket,nombreTemporal_Transformacion,tamanio_nombreTemporal_Transformacion,0);
    payload->nombreTemporal_Transformacion = nombreTemporal_Transformacion;

    recv(socket,&(payload->tamanio_nombreTemporal_ReduccionLocal),sizeof(int),0);
    int  tamanio_nombreTemporal_ReduccionLocal = payload->tamanio_nombreTemporal_ReduccionLocal;

    char* nombreTemporal_ReduccionLocal = malloc(tamanio_nombreTemporal_ReduccionLocal);
    recv(socket,nombreTemporal_ReduccionLocal,tamanio_nombreTemporal_ReduccionLocal,0);
    payload->nombreTemporal_ReduccionLocal = nombreTemporal_ReduccionLocal;

    return (void*)payload;
};

void* unpack_ORDEN_REDUCCIONGLOBAL(int socket){
    payload_ORDEN_REDUCCIONGLOBAL *payload= malloc(sizeof(payload_ORDEN_REDUCCIONGLOBAL));

    recv(socket,&(payload->PUERTO_Nodo),sizeof(int),0);

    recv(socket,&(payload->tamanio_IP_Nodo),sizeof(int),0);
    int  tamanio_IP_Nodo = payload->tamanio_IP_Nodo;

    char* IP_Nodo = malloc(tamanio_IP_Nodo);
    recv(socket,IP_Nodo,tamanio_IP_Nodo,0);
    payload->IP_Nodo = IP_Nodo;

    recv(socket,&(payload->tamanio_nombreTemporal_ReduccionLocal),sizeof(int),0);
    int  tamanio_nombreTemporal_ReduccionLocal = payload->tamanio_nombreTemporal_ReduccionLocal;

    char* nombreTemporal_ReduccionLocal = malloc(tamanio_nombreTemporal_ReduccionLocal);
    recv(socket,nombreTemporal_ReduccionLocal,tamanio_nombreTemporal_ReduccionLocal,0);
    payload->nombreTemporal_ReduccionLocal = nombreTemporal_ReduccionLocal;

    recv(socket,&(payload->tamanio_nombreTemporal_ReduccionGlobal),sizeof(int),0);
    int  tamanio_nombreTemporal_ReduccionGlobal = payload->tamanio_nombreTemporal_ReduccionGlobal;

    char* nombreTemporal_ReduccionGlobal = malloc(tamanio_nombreTemporal_ReduccionGlobal);
    recv(socket,nombreTemporal_ReduccionGlobal,tamanio_nombreTemporal_ReduccionGlobal,0);
    payload->nombreTemporal_ReduccionGlobal = nombreTemporal_ReduccionGlobal;

    recv(socket,&(payload->encargado),sizeof(int),0);

    return (void*)payload;
};

void* unpack_ORDEN_ALMACENAMIENTO(int socket){
    payload_ORDEN_ALMACENAMIENTO *payload= malloc(sizeof(payload_ORDEN_ALMACENAMIENTO));

    recv(socket,&(payload->tamanio_nombreTemporal_ReduccionGlobal),sizeof(int),0);
    int  tamanio_nombreTemporal_ReduccionGlobal = payload->tamanio_nombreTemporal_ReduccionGlobal;

    char* nombreTemporal_ReduccionGlobal = malloc(tamanio_nombreTemporal_ReduccionGlobal);
    recv(socket,nombreTemporal_ReduccionGlobal,tamanio_nombreTemporal_ReduccionGlobal,0);
    payload->nombreTemporal_ReduccionGlobal = nombreTemporal_ReduccionGlobal;

    return (void*)payload;
};

void* unpack_INFO_TRANSFORMACION(int socket){
    payload_INFO_TRANSFORMACION *payload= malloc(sizeof(payload_INFO_TRANSFORMACION));

    recv(socket,&(payload->PUERTO_Worker),sizeof(int),0);

    recv(socket,&(payload->tamanio_IP_Worker),sizeof(int),0);
    int  tamanio_IP_Worker = payload->tamanio_IP_Worker;

    char* IP_Worker = malloc(tamanio_IP_Worker);
    recv(socket,IP_Worker,tamanio_IP_Worker,0);
    payload->IP_Worker = IP_Worker;

    recv(socket,&(payload->bloque),sizeof(int),0);

    recv(socket,&(payload->bytesocupados),sizeof(int),0);

    recv(socket,&(payload->tamanio_nombreArchivoTemporal),sizeof(int),0);
    int  tamanio_nombreArchivoTemporal = payload->tamanio_nombreArchivoTemporal;

    char* nombreArchivoTemporal = malloc(tamanio_nombreArchivoTemporal);
    recv(socket,nombreArchivoTemporal,tamanio_nombreArchivoTemporal,0);
    payload->nombreArchivoTemporal = nombreArchivoTemporal;

    return (void*)payload;
};

void* unpack_INFO_REDUCCIONLOCAL(int socket){
    payload_INFO_REDUCCIONLOCAL *payload= malloc(sizeof(payload_INFO_REDUCCIONLOCAL));

    recv(socket,&(payload->PUERTO_Worker),sizeof(int),0);

    recv(socket,&(payload->tamanio_IP_Worker),sizeof(int),0);
    int  tamanio_IP_Worker = payload->tamanio_IP_Worker;

    char* IP_Worker = malloc(tamanio_IP_Worker);
    recv(socket,IP_Worker,tamanio_IP_Worker,0);
    payload->IP_Worker = IP_Worker;

    recv(socket,&(payload->tamanio_nombreTemporal_Transformacion),sizeof(int),0);
    int  tamanio_nombreTemporal_Transformacion = payload->tamanio_nombreTemporal_Transformacion;

    char* nombreTemporal_Transformacion = malloc(tamanio_nombreTemporal_Transformacion);
    recv(socket,nombreTemporal_Transformacion,tamanio_nombreTemporal_Transformacion,0);
    payload->nombreTemporal_Transformacion = nombreTemporal_Transformacion;

    recv(socket,&(payload->tamanio_nombreTemporal_ReduccionLocal),sizeof(int),0);
    int  tamanio_nombreTemporal_ReduccionLocal = payload->tamanio_nombreTemporal_ReduccionLocal;

    char* nombreTemporal_ReduccionLocal = malloc(tamanio_nombreTemporal_ReduccionLocal);
    recv(socket,nombreTemporal_ReduccionLocal,tamanio_nombreTemporal_ReduccionLocal,0);
    payload->nombreTemporal_ReduccionLocal = nombreTemporal_ReduccionLocal;

    return (void*)payload;
};

void* unpack_INFO_REDUCCIONGLOBAL(int socket){
    payload_INFO_REDUCCIONGLOBAL *payload= malloc(sizeof(payload_INFO_REDUCCIONGLOBAL));

    recv(socket,&(payload->PUERTO_Worker),sizeof(int),0);

    recv(socket,&(payload->tamanio_IP_Worker),sizeof(int),0);
    int  tamanio_IP_Worker = payload->tamanio_IP_Worker;

    char* IP_Worker = malloc(tamanio_IP_Worker);
    recv(socket,IP_Worker,tamanio_IP_Worker,0);
    payload->IP_Worker = IP_Worker;

    recv(socket,&(payload->tamanio_nombreTemporal_ReduccionLocal),sizeof(int),0);
    int  tamanio_nombreTemporal_ReduccionLocal = payload->tamanio_nombreTemporal_ReduccionLocal;

    char* nombreTemporal_ReduccionLocal = malloc(tamanio_nombreTemporal_ReduccionLocal);
    recv(socket,nombreTemporal_ReduccionLocal,tamanio_nombreTemporal_ReduccionLocal,0);
    payload->nombreTemporal_ReduccionLocal = nombreTemporal_ReduccionLocal;

    recv(socket,&(payload->tamanio_nombreTemporal_ReduccionGlobal),sizeof(int),0);
    int  tamanio_nombreTemporal_ReduccionGlobal = payload->tamanio_nombreTemporal_ReduccionGlobal;

    char* nombreTemporal_ReduccionGlobal = malloc(tamanio_nombreTemporal_ReduccionGlobal);
    recv(socket,nombreTemporal_ReduccionGlobal,tamanio_nombreTemporal_ReduccionGlobal,0);
    payload->nombreTemporal_ReduccionGlobal = nombreTemporal_ReduccionGlobal;

    recv(socket,&(payload->encargado),sizeof(int),0);

    return (void*)payload;
};

void* unpack_INFO_ALMACENAMIENTO(int socket){
    payload_INFO_ALMACENAMIENTO *payload= malloc(sizeof(payload_INFO_ALMACENAMIENTO));

    recv(socket,&(payload->PUERTO_Worker),sizeof(int),0);

    recv(socket,&(payload->tamanio_IP_Worker),sizeof(int),0);
    int  tamanio_IP_Worker = payload->tamanio_IP_Worker;

    char* IP_Worker = malloc(tamanio_IP_Worker);
    recv(socket,IP_Worker,tamanio_IP_Worker,0);
    payload->IP_Worker = IP_Worker;

    recv(socket,&(payload->tamanio_nombreTemporal_ReduccionGlobal),sizeof(int),0);
    int  tamanio_nombreTemporal_ReduccionGlobal = payload->tamanio_nombreTemporal_ReduccionGlobal;

    char* nombreTemporal_ReduccionGlobal = malloc(tamanio_nombreTemporal_ReduccionGlobal);
    recv(socket,nombreTemporal_ReduccionGlobal,tamanio_nombreTemporal_ReduccionGlobal,0);
    payload->nombreTemporal_ReduccionGlobal = nombreTemporal_ReduccionGlobal;

    return (void*)payload;
};

void* unpack_PETICION_NODO(int socket){
    payload_PETICION_NODO *payload= malloc(sizeof(payload_PETICION_NODO));

    recv(socket,&(payload->tamanio_nombreArchivo),sizeof(int),0);
    int  tamanio_nombreArchivo = payload->tamanio_nombreArchivo;

    char* nombreArchivo = malloc(tamanio_nombreArchivo);
    recv(socket,nombreArchivo,tamanio_nombreArchivo,0);
    payload->nombreArchivo = nombreArchivo;

    return (void*)payload;
};

void* unpack_NODO(int socket){
    payload_NODO *payload= malloc(sizeof(payload_NODO));

    recv(socket,&(payload->PUERTO_Nodo),sizeof(int),0);

    recv(socket,&(payload->tamanio_IP_Nodo),sizeof(int),0);
    int  tamanio_IP_Nodo = payload->tamanio_IP_Nodo;

    char* IP_Nodo = malloc(tamanio_IP_Nodo);
    recv(socket,IP_Nodo,tamanio_IP_Nodo,0);
    payload->IP_Nodo = IP_Nodo;

    recv(socket,&(payload->tamanio_nombreNodo),sizeof(int),0);
    int  tamanio_nombreNodo = payload->tamanio_nombreNodo;

    char* nombreNodo = malloc(tamanio_nombreNodo);
    recv(socket,nombreNodo,tamanio_nombreNodo,0);
    payload->nombreNodo = nombreNodo;

    return (void*)payload;
};

void* unpack_ARCHIVO(int socket){
    payload_ARCHIVO *payload= malloc(sizeof(payload_ARCHIVO));

    recv(socket,&(payload->tamanio_archivo),sizeof(uint64_t),0);
    uint64_t  tamanio_archivo = payload->tamanio_archivo;

    char* archivo = malloc(tamanio_archivo);
    recv(socket,archivo,tamanio_archivo,0);
    payload->archivo = archivo;

    return (void*)payload;
};
void* unpack_BLOQUE(int socket){
    payload_BLOQUE *payload= malloc(sizeof(payload_BLOQUE));

    recv(socket,&(payload->tamanio_bloque),sizeof(int),MSG_WAITALL);
    int  tamanio_bloque = payload->tamanio_bloque;

    char* bloque = malloc(tamanio_bloque);
    recv(socket,bloque,tamanio_bloque,MSG_WAITALL);
    payload->contenido = bloque;

    recv(socket,&(payload->numero_bloque),sizeof(int),MSG_WAITALL);

    return (void*)payload;
};
void* unpack_PRESENTACION_DATANODE(int socket){
    payload_PRESENTACION_DATANODE *payload= malloc(sizeof(payload_PRESENTACION_DATANODE));

    recv(socket,&(payload->pid),sizeof(int),0);

    recv(socket,&(payload->id_dataNode),sizeof(int),0);

    recv(socket,&(payload->cantidad_bloques),sizeof(int),0);

    return (void*)payload;
};

void* unpack_JOB(int socket){
    payload_JOB *payload= malloc(sizeof(payload_JOB));

    recv(socket,&(payload->id_master),sizeof(int),0);

    return (void*)payload;
};

void* unpack_RESPUESTA_MASTER(int socket){
    payload_RESPUESTA_MASTER *payload= malloc(sizeof(payload_RESPUESTA_MASTER));

    recv(socket,&(payload->id_master),sizeof(int),0);

    recv(socket,&(payload->id_nodo),sizeof(int),0);

    recv(socket,&(payload->bloque),sizeof(int),0);

    recv(socket,&(payload->estado),sizeof(int),0);

    return (void*)payload;
};

void* unpack_SCRIPT(int socket){
    payload_SCRIPT *payload= malloc(sizeof(payload_SCRIPT));

    recv(socket,&(payload->tamanio_contenido),sizeof(int),0);
    int  tamanio_contenido = payload->tamanio_contenido;

    char* contenido = malloc(tamanio_contenido);
    recv(socket,contenido,tamanio_contenido,0);
    payload->contenido = contenido;

    return (void*)payload;
};

void* unpack_PETICION_BLOQUE(int socket){
    payload_PETICION_BLOQUE *payload= malloc(sizeof(payload_PETICION_BLOQUE));

    recv(socket,&(payload->numero_bloque),sizeof(int),0);

    recv(socket,&(payload->tam_bloque),sizeof(int),0);

    return (void*)payload;
};

void* unpack_UBICACION_BLOQUE(int socket){
    payload_UBICACION_BLOQUE *payload= malloc(sizeof(payload_UBICACION_BLOQUE));

    recv(socket,&(payload->ip),sizeof(int),0);

    recv(socket,&(payload->puerto),sizeof(int),0);

    recv(socket,&(payload->numero_nodo),sizeof(int),0);

    recv(socket,&(payload->bloque_nodo),sizeof(int),0);

    recv(socket,&(payload->bloque_archivo),sizeof(int),0);

    recv(socket,&(payload->copia),sizeof(int),0);

    return (void*)payload;
};

void* unpack_TEMPORAL(int socket){
    payload_TEMPORAL *payload= malloc(sizeof(payload_TEMPORAL));

    recv(socket,&(payload->tamanio_contenido),sizeof(int),0);
    int  tamanio_contenido = payload->tamanio_contenido;

    char* contenido = malloc(tamanio_contenido);
    recv(socket,contenido,tamanio_contenido,0);
    payload->contenido = contenido;

    return (void*)payload;
};

void* unpack_PETICION_TEMPORAL(int socket){
    payload_PETICION_TEMPORAL *payload= malloc(sizeof(payload_PETICION_TEMPORAL));

    recv(socket,&(payload->tamanio_nombre),sizeof(int),0);
    int  tamanio_nombre = payload->tamanio_nombre;

    char* nombre = malloc(tamanio_nombre);
    recv(socket,nombre,tamanio_nombre,0);
    payload->nombre = nombre;

    return (void*)payload;
};

