/*
 * types.h
 * Generado automaticamente por ProtoCool
*/

#ifndef UTILIDADES_PROTOCOL_TYPES_H_
#define UTILIDADES_PROTOCOL_TYPES_H_

#include <stdint.h>

typedef enum {SOLICITUD_PROCESAMIENTO, SOLICITUD_REDUCCIONLOCAL, SOLICITUD_REDUCCIONGLOBAL, SOLICITUD_ALMACENAMIENTO, ORDEN_TRANSFORMACION, ORDEN_REDUCCIONLOCAL, ORDEN_REDUCCIONGLOBAL, ORDEN_ALMACENAMIENTO, INFO_TRANSFORMACION, INFO_REDUCCIONLOCAL, INFO_REDUCCIONGLOBAL, INFO_ALMACENAMIENTO, PEDIDO_NODO, NODO, FIN_LISTA, FIN_COMUNICACION,EJECUTABLE}HEADER_T;

typedef struct { 
    uint16_t tamanio_archivo;
    char* archivo;
}payload_EJECUTABLE;

typedef struct {
    uint16_t tamanio_nombreArchivo; 
    char* nombreArchivo; 
}payload_SOLICITUD_PROCESAMIENTO;

typedef struct { 
}payload_SOLICITUD_REDUCCIONLOCAL;

typedef struct { 
}payload_SOLICITUD_REDUCCIONGLOBAL;

typedef struct { 
}payload_SOLICITUD_ALMACENAMIENTO;

typedef struct { 
    uint16_t bloque; 
    uint16_t bytesocupados; 
    uint16_t tamanio_nombreArchivoTemporal; 
    char* nombreArchivoTemporal; 
}payload_ORDEN_TRANSFORMACION;

typedef struct { 
    uint16_t tamanio_nombreTemporal_Transformacion; 
    char* nombreTemporal_Transformacion; 
    uint16_t tamanio_nombreTemporal_ReduccionLocal; 
    char* nombreTemporal_ReduccionLocal; 
}payload_ORDEN_REDUCCIONLOCAL;

typedef struct { 
    uint16_t PUERTO_Nodo; 
    uint16_t tamanio_IP_Nodo; 
    char* IP_Nodo; 
    uint16_t tamanio_nombreTemporal_ReduccionLocal; 
    char* nombreTemporal_ReduccionLocal; 
    uint16_t tamanio_nombreTemporal_ReduccionGlobal; 
    char* nombreTemporal_ReduccionGlobal; 
    uint16_t encargado; 
}payload_ORDEN_REDUCCIONGLOBAL;

typedef struct { 
    uint16_t tamanio_nombreTemporal_ReduccionGlobal; 
    char* nombreTemporal_ReduccionGlobal; 
}payload_ORDEN_ALMACENAMIENTO;

typedef struct { 
    uint16_t PUERTO_Worker; 
    uint16_t tamanio_IP_Worker; 
    char* IP_Worker; 
    uint16_t bloque; 
    uint16_t bytesocupados; 
    uint16_t tamanio_nombreArchivoTemporal; 
    char* nombreArchivoTemporal; 
}payload_INFO_TRANSFORMACION;

typedef struct { 
    uint16_t PUERTO_Worker; 
    uint16_t tamanio_IP_Worker; 
    char* IP_Worker; 
    uint16_t tamanio_nombreTemporal_Transformacion; 
    char* nombreTemporal_Transformacion; 
    uint16_t tamanio_nombreTemporal_ReduccionLocal; 
    char* nombreTemporal_ReduccionLocal; 
}payload_INFO_REDUCCIONLOCAL;

typedef struct { 
    uint16_t PUERTO_Worker; 
    uint16_t tamanio_IP_Worker; 
    char* IP_Worker; 
    uint16_t tamanio_nombreTemporal_ReduccionLocal; 
    char* nombreTemporal_ReduccionLocal; 
    uint16_t tamanio_nombreTemporal_ReduccionGlobal; 
    char* nombreTemporal_ReduccionGlobal; 
    uint16_t encargado; 
}payload_INFO_REDUCCIONGLOBAL;

typedef struct { 
    uint16_t PUERTO_Worker; 
    uint16_t tamanio_IP_Worker; 
    char* IP_Worker; 
    uint16_t tamanio_nombreTemporal_ReduccionGlobal; 
    char* nombreTemporal_ReduccionGlobal; 
}payload_INFO_ALMACENAMIENTO;

typedef struct { 
}payload_FIN_LISTA;

typedef struct { 
}payload_FIN_COMUNICACION;

typedef struct {
    uint16_t tamanio_nombreArchivo;
    char* nombreArchivo;
}payload_PEDIDO_NODO;

typedef struct {
    uint16_t PUERTO_Nodo;
    uint16_t tamanio_IP_Nodo;
    char* IP_Nodo;
    uint16_t tamanio_nombreNodo;
    char* nombreNodo;
}payload_NODO;


#endif /* UTILIDADES_PROTOCOL_TYPES_H_ */
