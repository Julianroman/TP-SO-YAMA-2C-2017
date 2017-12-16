
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "types.h"

void* unpack_SOLICITUD_JOB(int socket){
    payload_SOLICITUD_JOB *payload= malloc(sizeof(payload_SOLICITUD_JOB));

    recv(socket,&(payload->tamanio_nombreArchivo),sizeof(int),MSG_WAITALL);
    int  tamanio_nombreArchivo = payload->tamanio_nombreArchivo;

    char* nombreArchivo = malloc(tamanio_nombreArchivo);
    recv(socket,nombreArchivo,tamanio_nombreArchivo,MSG_WAITALL);
    payload->nombreArchivo = nombreArchivo;

    return (void*)payload;
};

void* unpack_ORDEN_TRANSFORMACION(int socket){
    payload_ORDEN_TRANSFORMACION *payload= malloc(sizeof(payload_ORDEN_TRANSFORMACION));

    recv(socket,&(payload->bloque),sizeof(int),MSG_WAITALL);

    recv(socket,&(payload->bytesocupados),sizeof(int),MSG_WAITALL);

    recv(socket,&(payload->tamanio_nombreArchivoTemporal),sizeof(int),MSG_WAITALL);
    int  tamanio_nombreArchivoTemporal = payload->tamanio_nombreArchivoTemporal;

    char* nombreArchivoTemporal = malloc(tamanio_nombreArchivoTemporal);
    recv(socket,nombreArchivoTemporal,tamanio_nombreArchivoTemporal,MSG_WAITALL);
    payload->nombreArchivoTemporal = nombreArchivoTemporal;

    return (void*)payload;
};

void* unpack_ORDEN_REDUCCIONLOCAL(int socket){
    payload_ORDEN_REDUCCIONLOCAL *payload= malloc(sizeof(payload_ORDEN_REDUCCIONLOCAL));

    recv(socket,&(payload->tamanio_nombreTemporal_Transformacion),sizeof(int),MSG_WAITALL);
    int  tamanio_nombreTemporal_Transformacion = payload->tamanio_nombreTemporal_Transformacion;

    char* nombreTemporal_Transformacion = malloc(tamanio_nombreTemporal_Transformacion);
    recv(socket,nombreTemporal_Transformacion,tamanio_nombreTemporal_Transformacion,MSG_WAITALL);
    payload->nombreTemporal_Transformacion = nombreTemporal_Transformacion;

    recv(socket,&(payload->tamanio_nombreTemporal_ReduccionLocal),sizeof(int),MSG_WAITALL);
    int  tamanio_nombreTemporal_ReduccionLocal = payload->tamanio_nombreTemporal_ReduccionLocal;

    char* nombreTemporal_ReduccionLocal = malloc(tamanio_nombreTemporal_ReduccionLocal);
    recv(socket,nombreTemporal_ReduccionLocal,tamanio_nombreTemporal_ReduccionLocal,MSG_WAITALL);
    payload->nombreTemporal_ReduccionLocal = nombreTemporal_ReduccionLocal;

    return (void*)payload;
};

void* unpack_ORDEN_REDUCCIONGLOBAL(int socket){
    payload_ORDEN_REDUCCIONGLOBAL *payload= malloc(sizeof(payload_ORDEN_REDUCCIONGLOBAL));

    recv(socket,&(payload->PUERTO_Nodo),sizeof(int),MSG_WAITALL);

    recv(socket,&(payload->tamanio_IP_Nodo),sizeof(int),MSG_WAITALL);
    int  tamanio_IP_Nodo = payload->tamanio_IP_Nodo;

    char* IP_Nodo = malloc(tamanio_IP_Nodo);
    recv(socket,IP_Nodo,tamanio_IP_Nodo,MSG_WAITALL);
    payload->IP_Nodo = IP_Nodo;

    recv(socket,&(payload->tamanio_nombreTemporal_ReduccionLocal),sizeof(int),MSG_WAITALL);
    int  tamanio_nombreTemporal_ReduccionLocal = payload->tamanio_nombreTemporal_ReduccionLocal;

    char* nombreTemporal_ReduccionLocal = malloc(tamanio_nombreTemporal_ReduccionLocal);
    recv(socket,nombreTemporal_ReduccionLocal,tamanio_nombreTemporal_ReduccionLocal,MSG_WAITALL);
    payload->nombreTemporal_ReduccionLocal = nombreTemporal_ReduccionLocal;

    recv(socket,&(payload->tamanio_nombreTemporal_ReduccionGlobal),sizeof(int),MSG_WAITALL);
    int  tamanio_nombreTemporal_ReduccionGlobal = payload->tamanio_nombreTemporal_ReduccionGlobal;

    char* nombreTemporal_ReduccionGlobal = malloc(tamanio_nombreTemporal_ReduccionGlobal);
    recv(socket,nombreTemporal_ReduccionGlobal,tamanio_nombreTemporal_ReduccionGlobal,MSG_WAITALL);
    payload->nombreTemporal_ReduccionGlobal = nombreTemporal_ReduccionGlobal;

    recv(socket,&(payload->encargado),sizeof(int),MSG_WAITALL);

    return (void*)payload;
};

void* unpack_ORDEN_ALMACENAMIENTO(int socket){
    payload_ORDEN_ALMACENAMIENTO *payload= malloc(sizeof(payload_ORDEN_ALMACENAMIENTO));

    recv(socket,&(payload->tamanio_rutaAlmacenamiento),sizeof(int),MSG_WAITALL);
    int  tamanio_rutaAlmacenamiento = payload->tamanio_rutaAlmacenamiento;

    char* rutaAlmacenamiento = malloc(tamanio_rutaAlmacenamiento);
    recv(socket,rutaAlmacenamiento,tamanio_rutaAlmacenamiento,MSG_WAITALL);
    payload->rutaAlmacenamiento = rutaAlmacenamiento;

    recv(socket,&(payload->tamanio_nombreAlamcenamiento),sizeof(int),MSG_WAITALL);
    int  tamanio_nombreAlamcenamiento = payload->tamanio_nombreAlamcenamiento;

    char* nombreAlamcenamiento = malloc(tamanio_nombreAlamcenamiento);
    recv(socket,nombreAlamcenamiento,tamanio_nombreAlamcenamiento,MSG_WAITALL);
    payload->nombreAlamcenamiento = nombreAlamcenamiento;

    recv(socket,&(payload->tamanio_nombreTemporal_ReduccionGlobal),sizeof(int),MSG_WAITALL);
    int  tamanio_nombreTemporal_ReduccionGlobal = payload->tamanio_nombreTemporal_ReduccionGlobal;

    char* nombreTemporal_ReduccionGlobal = malloc(tamanio_nombreTemporal_ReduccionGlobal);
    recv(socket,nombreTemporal_ReduccionGlobal,tamanio_nombreTemporal_ReduccionGlobal,MSG_WAITALL);
    payload->nombreTemporal_ReduccionGlobal = nombreTemporal_ReduccionGlobal;

    return (void*)payload;
};

void* unpack_INFO_TRANSFORMACION(int socket){
    payload_INFO_TRANSFORMACION *payload= malloc(sizeof(payload_INFO_TRANSFORMACION));

    recv(socket,&(payload->PUERTO_Worker),sizeof(int),MSG_WAITALL);

    recv(socket,&(payload->tamanio_IP_Worker),sizeof(int),MSG_WAITALL);
    int  tamanio_IP_Worker = payload->tamanio_IP_Worker;

    char* IP_Worker = malloc(tamanio_IP_Worker);
    recv(socket,IP_Worker,tamanio_IP_Worker,MSG_WAITALL);
    payload->IP_Worker = IP_Worker;

    recv(socket,&(payload->bloque),sizeof(int),MSG_WAITALL);

    recv(socket,&(payload->bytesocupados),sizeof(int),MSG_WAITALL);

    recv(socket,&(payload->tamanio_nombreArchivoTemporal),sizeof(int),MSG_WAITALL);
    int  tamanio_nombreArchivoTemporal = payload->tamanio_nombreArchivoTemporal;

    char* nombreArchivoTemporal = malloc(tamanio_nombreArchivoTemporal);
    recv(socket,nombreArchivoTemporal,tamanio_nombreArchivoTemporal,MSG_WAITALL);
    payload->nombreArchivoTemporal = nombreArchivoTemporal;

    recv(socket,&(payload->ID_Nodo),sizeof(int),MSG_WAITALL);

    return (void*)payload;
};

void* unpack_INFO_REDUCCIONLOCAL(int socket){
    payload_INFO_REDUCCIONLOCAL *payload= malloc(sizeof(payload_INFO_REDUCCIONLOCAL));

    recv(socket,&(payload->PUERTO_Worker),sizeof(int),MSG_WAITALL);

    recv(socket,&(payload->tamanio_IP_Worker),sizeof(int),MSG_WAITALL);
    int  tamanio_IP_Worker = payload->tamanio_IP_Worker;

    char* IP_Worker = malloc(tamanio_IP_Worker);
    recv(socket,IP_Worker,tamanio_IP_Worker,MSG_WAITALL);
    payload->IP_Worker = IP_Worker;

    recv(socket,&(payload->tamanio_nombreTemporal_Transformacion),sizeof(int),MSG_WAITALL);
    int  tamanio_nombreTemporal_Transformacion = payload->tamanio_nombreTemporal_Transformacion;

    char* nombreTemporal_Transformacion = malloc(tamanio_nombreTemporal_Transformacion);
    recv(socket,nombreTemporal_Transformacion,tamanio_nombreTemporal_Transformacion,MSG_WAITALL);
    payload->nombreTemporal_Transformacion = nombreTemporal_Transformacion;

    recv(socket,&(payload->tamanio_nombreTemporal_ReduccionLocal),sizeof(int),MSG_WAITALL);
    int  tamanio_nombreTemporal_ReduccionLocal = payload->tamanio_nombreTemporal_ReduccionLocal;

    char* nombreTemporal_ReduccionLocal = malloc(tamanio_nombreTemporal_ReduccionLocal);
    recv(socket,nombreTemporal_ReduccionLocal,tamanio_nombreTemporal_ReduccionLocal,MSG_WAITALL);
    payload->nombreTemporal_ReduccionLocal = nombreTemporal_ReduccionLocal;

    recv(socket,&(payload->ID_Nodo),sizeof(int),MSG_WAITALL);

    return (void*)payload;
};

void* unpack_INFO_REDUCCIONGLOBAL(int socket){
    payload_INFO_REDUCCIONGLOBAL *payload= malloc(sizeof(payload_INFO_REDUCCIONGLOBAL));

    recv(socket,&(payload->PUERTO_Worker),sizeof(int),MSG_WAITALL);

    recv(socket,&(payload->tamanio_IP_Worker),sizeof(int),MSG_WAITALL);
    int  tamanio_IP_Worker = payload->tamanio_IP_Worker;

    char* IP_Worker = malloc(tamanio_IP_Worker);
    recv(socket,IP_Worker,tamanio_IP_Worker,MSG_WAITALL);
    payload->IP_Worker = IP_Worker;

    recv(socket,&(payload->tamanio_nombreTemporal_ReduccionLocal),sizeof(int),MSG_WAITALL);
    int  tamanio_nombreTemporal_ReduccionLocal = payload->tamanio_nombreTemporal_ReduccionLocal;

    char* nombreTemporal_ReduccionLocal = malloc(tamanio_nombreTemporal_ReduccionLocal);
    recv(socket,nombreTemporal_ReduccionLocal,tamanio_nombreTemporal_ReduccionLocal,MSG_WAITALL);
    payload->nombreTemporal_ReduccionLocal = nombreTemporal_ReduccionLocal;

    recv(socket,&(payload->tamanio_nombreTemporal_ReduccionGlobal),sizeof(int),MSG_WAITALL);
    int  tamanio_nombreTemporal_ReduccionGlobal = payload->tamanio_nombreTemporal_ReduccionGlobal;

    char* nombreTemporal_ReduccionGlobal = malloc(tamanio_nombreTemporal_ReduccionGlobal);
    recv(socket,nombreTemporal_ReduccionGlobal,tamanio_nombreTemporal_ReduccionGlobal,MSG_WAITALL);
    payload->nombreTemporal_ReduccionGlobal = nombreTemporal_ReduccionGlobal;

    recv(socket,&(payload->encargado),sizeof(int),MSG_WAITALL);

    recv(socket,&(payload->ID_Nodo),sizeof(int),MSG_WAITALL);

    return (void*)payload;
};

void* unpack_INFO_ALMACENAMIENTO(int socket){
    payload_INFO_ALMACENAMIENTO *payload= malloc(sizeof(payload_INFO_ALMACENAMIENTO));

    recv(socket,&(payload->PUERTO_Worker),sizeof(int),MSG_WAITALL);

    recv(socket,&(payload->tamanio_IP_Worker),sizeof(int),MSG_WAITALL);
    int  tamanio_IP_Worker = payload->tamanio_IP_Worker;

    char* IP_Worker = malloc(tamanio_IP_Worker);
    recv(socket,IP_Worker,tamanio_IP_Worker,MSG_WAITALL);
    payload->IP_Worker = IP_Worker;

    recv(socket,&(payload->tamanio_nombreTemporal_ReduccionGlobal),sizeof(int),MSG_WAITALL);
    int  tamanio_nombreTemporal_ReduccionGlobal = payload->tamanio_nombreTemporal_ReduccionGlobal;

    char* nombreTemporal_ReduccionGlobal = malloc(tamanio_nombreTemporal_ReduccionGlobal);
    recv(socket,nombreTemporal_ReduccionGlobal,tamanio_nombreTemporal_ReduccionGlobal,MSG_WAITALL);
    payload->nombreTemporal_ReduccionGlobal = nombreTemporal_ReduccionGlobal;

    recv(socket,&(payload->ID_Nodo),sizeof(int),MSG_WAITALL);

    return (void*)payload;
};

void* unpack_PETICION_NODO(int socket){
    payload_PETICION_NODO *payload= malloc(sizeof(payload_PETICION_NODO));

    recv(socket,&(payload->tamanio_nombreArchivo),sizeof(int),MSG_WAITALL);
    int  tamanio_nombreArchivo = payload->tamanio_nombreArchivo;

    char* nombreArchivo = malloc(tamanio_nombreArchivo);
    recv(socket,nombreArchivo,tamanio_nombreArchivo,MSG_WAITALL);
    payload->nombreArchivo = nombreArchivo;

    return (void*)payload;
};

void* unpack_NODO(int socket){
    payload_NODO *payload= malloc(sizeof(payload_NODO));

    recv(socket,&(payload->PUERTO_Nodo),sizeof(int),MSG_WAITALL);

    recv(socket,&(payload->tamanio_IP_Nodo),sizeof(int),MSG_WAITALL);
    int  tamanio_IP_Nodo = payload->tamanio_IP_Nodo;

    char* IP_Nodo = malloc(tamanio_IP_Nodo);
    recv(socket,IP_Nodo,tamanio_IP_Nodo,MSG_WAITALL);
    payload->IP_Nodo = IP_Nodo;

    recv(socket,&(payload->tamanio_nombreNodo),sizeof(int),MSG_WAITALL);
    int  tamanio_nombreNodo = payload->tamanio_nombreNodo;

    char* nombreNodo = malloc(tamanio_nombreNodo);
    recv(socket,nombreNodo,tamanio_nombreNodo,MSG_WAITALL);
    payload->nombreNodo = nombreNodo;

    return (void*)payload;
};

void* unpack_ARCHIVO(int socket){
    payload_ARCHIVO *payload= malloc(sizeof(payload_ARCHIVO));

    recv(socket,&(payload->tamanio_archivo),sizeof(uint64_t),MSG_WAITALL);
    uint64_t  tamanio_archivo = payload->tamanio_archivo;

    char* archivo = malloc(tamanio_archivo);
    recv(socket,archivo,tamanio_archivo,MSG_WAITALL);
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

    recv(socket,&(payload->id_dataNode),sizeof(int),MSG_WAITALL);

    recv(socket,&(payload->cantidad_bloques),sizeof(int),MSG_WAITALL);

    recv(socket,&(payload->tamanio_ipDatanode),sizeof(int),MSG_WAITALL);
    int  tamanio_ipDatanode = payload->tamanio_ipDatanode;

    char* ipDatanode = malloc(tamanio_ipDatanode);
    recv(socket,ipDatanode,tamanio_ipDatanode,MSG_WAITALL);
    payload->ipDatanode = ipDatanode;

    return (void*)payload;
};

void* unpack_JOB(int socket){
    payload_JOB *payload= malloc(sizeof(payload_JOB));

    recv(socket,&(payload->id_master),sizeof(int),MSG_WAITALL);

    return (void*)payload;
};

void* unpack_RESPUESTA_MASTER(int socket){
    payload_RESPUESTA_MASTER *payload= malloc(sizeof(payload_RESPUESTA_MASTER));

    recv(socket,&(payload->id_master),sizeof(int),MSG_WAITALL);

    recv(socket,&(payload->id_nodo),sizeof(int),MSG_WAITALL);

    recv(socket,&(payload->bloque),sizeof(int),MSG_WAITALL);

    recv(socket,&(payload->estado),sizeof(int),MSG_WAITALL);

    return (void*)payload;
};

void* unpack_SCRIPT(int socket){
    payload_SCRIPT *payload= malloc(sizeof(payload_SCRIPT));

    recv(socket,&(payload->tamanio_contenido),sizeof(int),MSG_WAITALL);
    int  tamanio_contenido = payload->tamanio_contenido;

    char* contenido = malloc(tamanio_contenido);
    recv(socket,contenido,tamanio_contenido,MSG_WAITALL);
    payload->contenido = contenido;

    return (void*)payload;
};

void* unpack_PETICION_BLOQUE(int socket){
    payload_PETICION_BLOQUE *payload= malloc(sizeof(payload_PETICION_BLOQUE));

    recv(socket,&(payload->numero_bloque),sizeof(int),MSG_WAITALL);

    recv(socket,&(payload->tam_bloque),sizeof(int),MSG_WAITALL);

    return (void*)payload;
};

void* unpack_UBICACION_BLOQUE(int socket){
    payload_UBICACION_BLOQUE *payload= malloc(sizeof(payload_UBICACION_BLOQUE));

    recv(socket,&(payload->tamanio_ip),sizeof(int),MSG_WAITALL);
    int  tamanio_ip = payload->tamanio_ip;

    char* ip = malloc(tamanio_ip);
    recv(socket,ip,tamanio_ip,MSG_WAITALL);
    payload->ip = ip;

    recv(socket,&(payload->puerto),sizeof(int),MSG_WAITALL);

    recv(socket,&(payload->numero_nodo),sizeof(int),MSG_WAITALL);

    recv(socket,&(payload->bloque_nodo),sizeof(int),MSG_WAITALL);

    recv(socket,&(payload->bloque_archivo),sizeof(int),MSG_WAITALL);

    recv(socket,&(payload->copia),sizeof(int),MSG_WAITALL);

    recv(socket,&(payload->tam_bloque),sizeof(int),MSG_WAITALL);

    return (void*)payload;
};

void* unpack_TEMPORAL(int socket){
    payload_TEMPORAL *payload= malloc(sizeof(payload_TEMPORAL));

    recv(socket,&(payload->tamanio_contenido),sizeof(int),MSG_WAITALL);
    int  tamanio_contenido = payload->tamanio_contenido;

    char* contenido = malloc(tamanio_contenido);
    recv(socket,contenido,tamanio_contenido,MSG_WAITALL);
    payload->contenido = contenido;

    return (void*)payload;
};

void* unpack_PETICION_TEMPORAL(int socket){
    payload_PETICION_TEMPORAL *payload= malloc(sizeof(payload_PETICION_TEMPORAL));

    recv(socket,&(payload->tamanio_nombre),sizeof(int),MSG_WAITALL);
    int  tamanio_nombre = payload->tamanio_nombre;

    char* nombre = malloc(tamanio_nombre);
    recv(socket,nombre,tamanio_nombre,MSG_WAITALL);
    payload->nombre = nombre;

    return (void*)payload;
};

void* unpack_ALMACENAR_ARCHIVO(int socket){

    payload_ALMACENAR_ARCHIVO *payload= malloc(sizeof(payload_ALMACENAR_ARCHIVO));

    recv(socket,&(payload->tamanio_contenido),sizeof(int),MSG_WAITALL);
    int  tamanio_contenido = payload->tamanio_contenido;

    char* contenido = malloc(tamanio_contenido);
    recv(socket,contenido,tamanio_contenido,MSG_WAITALL);
    payload->contenido = contenido;

    recv(socket,&(payload->tamanio_pathDestino),sizeof(int),MSG_WAITALL);
    int  tamanio_pathDestino = payload->tamanio_pathDestino;

    char* pathDestino = malloc(tamanio_pathDestino);
    recv(socket,pathDestino,tamanio_pathDestino,MSG_WAITALL);
    payload->pathDestino = pathDestino;

    recv(socket,&(payload->tamanio_nombre),sizeof(int),MSG_WAITALL);
    int  tamanio_nombre = payload->tamanio_nombre;

    char* nombre = malloc(tamanio_nombre);
    recv(socket,nombre,tamanio_nombre,MSG_WAITALL);
    payload->nombre = nombre;

    recv(socket,&(payload->tamanio_tipo),sizeof(int),MSG_WAITALL);
    int  tamanio_tipo = payload->tamanio_tipo;

    char* tipo = malloc(tamanio_tipo);
    recv(socket,tipo,tamanio_tipo,MSG_WAITALL);
    payload->tipo = tipo;

    return (void*)payload;


};
