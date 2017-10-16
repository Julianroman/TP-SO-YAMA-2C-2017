/*
 * packers.c
 * Generado automaticamente por ProtoCool
*/

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "types.h"

char* pack_SOLICITUD_PROCESAMIENTO(payload_SOLICITUD_PROCESAMIENTO payload,int* tamanio_paquete){
    int tamanio_total = sizeof(HEADER_T) + sizeof(uint16_t) + (payload.tamanio_nombreArchivo);
    char* paquete = malloc(tamanio_total);

    int offset = 0;
    int tamanio_envio;
    HEADER_T cabecera = SOLICITUD_PROCESAMIENTO;
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

char* pack_SOLICITUD_REDUCCIONLOCAL(payload_SOLICITUD_REDUCCIONLOCAL payload,int* tamanio_paquete){
    int tamanio_total = sizeof(HEADER_T);
    char* paquete = malloc(tamanio_total);

    int offset = 0;
    int tamanio_envio;
    HEADER_T cabecera = SOLICITUD_REDUCCIONLOCAL;
    tamanio_envio = (sizeof(HEADER_T));
    memcpy(paquete+offset,&cabecera,tamanio_envio);
    offset += tamanio_envio;

    (* tamanio_paquete) = tamanio_total;
    return paquete;
};

char* pack_SOLICITUD_REDUCCIONGLOBAL(payload_SOLICITUD_REDUCCIONGLOBAL payload,int* tamanio_paquete){
    int tamanio_total = sizeof(HEADER_T);
    char* paquete = malloc(tamanio_total);

    int offset = 0;
    int tamanio_envio;
    HEADER_T cabecera = SOLICITUD_REDUCCIONGLOBAL;
    tamanio_envio = (sizeof(HEADER_T));
    memcpy(paquete+offset,&cabecera,tamanio_envio);
    offset += tamanio_envio;

    (* tamanio_paquete) = tamanio_total;
    return paquete;
};

char* pack_SOLICITUD_ALMACENAMIENTO(payload_SOLICITUD_ALMACENAMIENTO payload,int* tamanio_paquete){
    int tamanio_total = sizeof(HEADER_T);
    char* paquete = malloc(tamanio_total);

    int offset = 0;
    int tamanio_envio;
    HEADER_T cabecera = SOLICITUD_ALMACENAMIENTO;
    tamanio_envio = (sizeof(HEADER_T));
    memcpy(paquete+offset,&cabecera,tamanio_envio);
    offset += tamanio_envio;

    (* tamanio_paquete) = tamanio_total;
    return paquete;
};

char* pack_ORDEN_TRANSFORMACION(payload_ORDEN_TRANSFORMACION payload,int* tamanio_paquete){
    int tamanio_total = sizeof(HEADER_T) + sizeof(uint16_t) + sizeof(uint32_t) + sizeof(uint16_t) + (payload.tamanio_nombreArchivoTemporal);
    char* paquete = malloc(tamanio_total);

    int offset = 0;
    int tamanio_envio;
    HEADER_T cabecera = ORDEN_TRANSFORMACION;
    tamanio_envio = (sizeof(HEADER_T));
    memcpy(paquete+offset,&cabecera,tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(uint16_t);
    memcpy(paquete+offset,&(payload.bloque),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(uint32_t);
    memcpy(paquete+offset,&(payload.bytesocupados),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(uint16_t);
    memcpy(paquete+offset,&(payload.tamanio_nombreArchivoTemporal),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = (payload.tamanio_nombreArchivoTemporal);
    memcpy(paquete+offset,payload.nombreArchivoTemporal,tamanio_envio);
    offset += tamanio_envio;

    (* tamanio_paquete) = tamanio_total;
    return paquete;
};

char* pack_ORDEN_REDUCCIONLOCAL(payload_ORDEN_REDUCCIONLOCAL payload,int* tamanio_paquete){
    int tamanio_total = sizeof(HEADER_T) + sizeof(uint16_t) + (payload.tamanio_nombreTemporal_Transformacion) + sizeof(uint16_t) + (payload.tamanio_nombreTemporal_ReduccionLocal);
    char* paquete = malloc(tamanio_total);

    int offset = 0;
    int tamanio_envio;
    HEADER_T cabecera = ORDEN_REDUCCIONLOCAL;
    tamanio_envio = (sizeof(HEADER_T));
    memcpy(paquete+offset,&cabecera,tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(uint16_t);
    memcpy(paquete+offset,&(payload.tamanio_nombreTemporal_Transformacion),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = (payload.tamanio_nombreTemporal_Transformacion);
    memcpy(paquete+offset,payload.nombreTemporal_Transformacion,tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(uint16_t);
    memcpy(paquete+offset,&(payload.tamanio_nombreTemporal_ReduccionLocal),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = (payload.tamanio_nombreTemporal_ReduccionLocal);
    memcpy(paquete+offset,payload.nombreTemporal_ReduccionLocal,tamanio_envio);
    offset += tamanio_envio;

    (* tamanio_paquete) = tamanio_total;
    return paquete;
};

char* pack_ORDEN_REDUCCIONGLOBAL(payload_ORDEN_REDUCCIONGLOBAL payload,int* tamanio_paquete){
    int tamanio_total = sizeof(HEADER_T) + sizeof(uint16_t) + sizeof(uint16_t) + (payload.tamanio_IP_Nodo) + sizeof(uint16_t) + (payload.tamanio_nombreTemporal_ReduccionLocal) + sizeof(uint16_t) + (payload.tamanio_nombreTemporal_ReduccionGlobal) + sizeof(uint16_t);
    char* paquete = malloc(tamanio_total);

    int offset = 0;
    int tamanio_envio;
    HEADER_T cabecera = ORDEN_REDUCCIONGLOBAL;
    tamanio_envio = (sizeof(HEADER_T));
    memcpy(paquete+offset,&cabecera,tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(uint16_t);
    memcpy(paquete+offset,&(payload.PUERTO_Nodo),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(uint16_t);
    memcpy(paquete+offset,&(payload.tamanio_IP_Nodo),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = (payload.tamanio_IP_Nodo);
    memcpy(paquete+offset,payload.IP_Nodo,tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(uint16_t);
    memcpy(paquete+offset,&(payload.tamanio_nombreTemporal_ReduccionLocal),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = (payload.tamanio_nombreTemporal_ReduccionLocal);
    memcpy(paquete+offset,payload.nombreTemporal_ReduccionLocal,tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(uint16_t);
    memcpy(paquete+offset,&(payload.tamanio_nombreTemporal_ReduccionGlobal),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = (payload.tamanio_nombreTemporal_ReduccionGlobal);
    memcpy(paquete+offset,payload.nombreTemporal_ReduccionGlobal,tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(uint16_t);
    memcpy(paquete+offset,&(payload.encargado),tamanio_envio);
    offset += tamanio_envio;

    (* tamanio_paquete) = tamanio_total;
    return paquete;
};

char* pack_ORDEN_ALMACENAMIENTO(payload_ORDEN_ALMACENAMIENTO payload,int* tamanio_paquete){
    int tamanio_total = sizeof(HEADER_T) + sizeof(uint16_t) + (payload.tamanio_nombreTemporal_ReduccionGlobal);
    char* paquete = malloc(tamanio_total);

    int offset = 0;
    int tamanio_envio;
    HEADER_T cabecera = ORDEN_ALMACENAMIENTO;
    tamanio_envio = (sizeof(HEADER_T));
    memcpy(paquete+offset,&cabecera,tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(uint16_t);
    memcpy(paquete+offset,&(payload.tamanio_nombreTemporal_ReduccionGlobal),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = (payload.tamanio_nombreTemporal_ReduccionGlobal);
    memcpy(paquete+offset,payload.nombreTemporal_ReduccionGlobal,tamanio_envio);
    offset += tamanio_envio;

    (* tamanio_paquete) = tamanio_total;
    return paquete;
};

char* pack_INFO_TRANSFORMACION(payload_INFO_TRANSFORMACION payload,int* tamanio_paquete){
    int tamanio_total = sizeof(HEADER_T) + sizeof(uint16_t) + sizeof(uint16_t) + (payload.tamanio_IP_Worker) + sizeof(uint16_t) + sizeof(uint32_t) + sizeof(uint16_t) + (payload.tamanio_nombreArchivoTemporal);
    char* paquete = malloc(tamanio_total);

    int offset = 0;
    int tamanio_envio;
    HEADER_T cabecera = INFO_TRANSFORMACION;
    tamanio_envio = (sizeof(HEADER_T));
    memcpy(paquete+offset,&cabecera,tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(uint16_t);
    memcpy(paquete+offset,&(payload.PUERTO_Worker),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(uint16_t);
    memcpy(paquete+offset,&(payload.tamanio_IP_Worker),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = (payload.tamanio_IP_Worker);
    memcpy(paquete+offset,payload.IP_Worker,tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(uint16_t);
    memcpy(paquete+offset,&(payload.bloque),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(uint32_t);
    memcpy(paquete+offset,&(payload.bytesocupados),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(uint16_t);
    memcpy(paquete+offset,&(payload.tamanio_nombreArchivoTemporal),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = (payload.tamanio_nombreArchivoTemporal);
    memcpy(paquete+offset,payload.nombreArchivoTemporal,tamanio_envio);
    offset += tamanio_envio;

    (* tamanio_paquete) = tamanio_total;
    return paquete;
};

char* pack_INFO_REDUCCIONLOCAL(payload_INFO_REDUCCIONLOCAL payload,int* tamanio_paquete){
    int tamanio_total = sizeof(HEADER_T) + sizeof(uint16_t) + sizeof(uint16_t) + (payload.tamanio_IP_Worker) + sizeof(uint16_t) + (payload.tamanio_nombreTemporal_Transformacion) + sizeof(uint16_t) + (payload.tamanio_nombreTemporal_ReduccionLocal);
    char* paquete = malloc(tamanio_total);

    int offset = 0;
    int tamanio_envio;
    HEADER_T cabecera = INFO_REDUCCIONLOCAL;
    tamanio_envio = (sizeof(HEADER_T));
    memcpy(paquete+offset,&cabecera,tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(uint16_t);
    memcpy(paquete+offset,&(payload.PUERTO_Worker),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(uint16_t);
    memcpy(paquete+offset,&(payload.tamanio_IP_Worker),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = (payload.tamanio_IP_Worker);
    memcpy(paquete+offset,payload.IP_Worker,tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(uint16_t);
    memcpy(paquete+offset,&(payload.tamanio_nombreTemporal_Transformacion),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = (payload.tamanio_nombreTemporal_Transformacion);
    memcpy(paquete+offset,payload.nombreTemporal_Transformacion,tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(uint16_t);
    memcpy(paquete+offset,&(payload.tamanio_nombreTemporal_ReduccionLocal),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = (payload.tamanio_nombreTemporal_ReduccionLocal);
    memcpy(paquete+offset,payload.nombreTemporal_ReduccionLocal,tamanio_envio);
    offset += tamanio_envio;

    (* tamanio_paquete) = tamanio_total;
    return paquete;
};

char* pack_INFO_REDUCCIONGLOBAL(payload_INFO_REDUCCIONGLOBAL payload,int* tamanio_paquete){
    int tamanio_total = sizeof(HEADER_T) + sizeof(uint16_t) + sizeof(uint16_t) + (payload.tamanio_IP_Worker) + sizeof(uint16_t) + (payload.tamanio_nombreTemporal_ReduccionLocal) + sizeof(uint16_t) + (payload.tamanio_nombreTemporal_ReduccionGlobal) + sizeof(uint16_t);
    char* paquete = malloc(tamanio_total);

    int offset = 0;
    int tamanio_envio;
    HEADER_T cabecera = INFO_REDUCCIONGLOBAL;
    tamanio_envio = (sizeof(HEADER_T));
    memcpy(paquete+offset,&cabecera,tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(uint16_t);
    memcpy(paquete+offset,&(payload.PUERTO_Worker),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(uint16_t);
    memcpy(paquete+offset,&(payload.tamanio_IP_Worker),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = (payload.tamanio_IP_Worker);
    memcpy(paquete+offset,payload.IP_Worker,tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(uint16_t);
    memcpy(paquete+offset,&(payload.tamanio_nombreTemporal_ReduccionLocal),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = (payload.tamanio_nombreTemporal_ReduccionLocal);
    memcpy(paquete+offset,payload.nombreTemporal_ReduccionLocal,tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(uint16_t);
    memcpy(paquete+offset,&(payload.tamanio_nombreTemporal_ReduccionGlobal),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = (payload.tamanio_nombreTemporal_ReduccionGlobal);
    memcpy(paquete+offset,payload.nombreTemporal_ReduccionGlobal,tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(uint16_t);
    memcpy(paquete+offset,&(payload.encargado),tamanio_envio);
    offset += tamanio_envio;

    (* tamanio_paquete) = tamanio_total;
    return paquete;
};

char* pack_INFO_ALMACENAMIENTO(payload_INFO_ALMACENAMIENTO payload,int* tamanio_paquete){
    int tamanio_total = sizeof(HEADER_T) + sizeof(uint16_t) + sizeof(uint16_t) + (payload.tamanio_IP_Worker) + sizeof(uint16_t) + (payload.tamanio_nombreTemporal_ReduccionGlobal);
    char* paquete = malloc(tamanio_total);

    int offset = 0;
    int tamanio_envio;
    HEADER_T cabecera = INFO_ALMACENAMIENTO;
    tamanio_envio = (sizeof(HEADER_T));
    memcpy(paquete+offset,&cabecera,tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(uint16_t);
    memcpy(paquete+offset,&(payload.PUERTO_Worker),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(uint16_t);
    memcpy(paquete+offset,&(payload.tamanio_IP_Worker),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = (payload.tamanio_IP_Worker);
    memcpy(paquete+offset,payload.IP_Worker,tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(uint16_t);
    memcpy(paquete+offset,&(payload.tamanio_nombreTemporal_ReduccionGlobal),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = (payload.tamanio_nombreTemporal_ReduccionGlobal);
    memcpy(paquete+offset,payload.nombreTemporal_ReduccionGlobal,tamanio_envio);
    offset += tamanio_envio;

    (* tamanio_paquete) = tamanio_total;
    return paquete;
};

char* pack_PETICION_NODO(payload_PETICION_NODO payload,int* tamanio_paquete){
    int tamanio_total = sizeof(HEADER_T) + sizeof(uint16_t) + (payload.tamanio_nombreArchivo);
    char* paquete = malloc(tamanio_total);

    int offset = 0;
    int tamanio_envio;
    HEADER_T cabecera = PETICION_NODO;
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

char* pack_NODO(payload_NODO payload,int* tamanio_paquete){
    int tamanio_total = sizeof(HEADER_T) + sizeof(uint16_t) + sizeof(uint16_t) + (payload.tamanio_IP_Nodo) + sizeof(uint16_t) + (payload.tamanio_nombreNodo);
    char* paquete = malloc(tamanio_total);

    int offset = 0;
    int tamanio_envio;
    HEADER_T cabecera = NODO;
    tamanio_envio = (sizeof(HEADER_T));
    memcpy(paquete+offset,&cabecera,tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(uint16_t);
    memcpy(paquete+offset,&(payload.PUERTO_Nodo),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(uint16_t);
    memcpy(paquete+offset,&(payload.tamanio_IP_Nodo),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = (payload.tamanio_IP_Nodo);
    memcpy(paquete+offset,payload.IP_Nodo,tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(uint16_t);
    memcpy(paquete+offset,&(payload.tamanio_nombreNodo),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = (payload.tamanio_nombreNodo);
    memcpy(paquete+offset,payload.nombreNodo,tamanio_envio);
    offset += tamanio_envio;

    (* tamanio_paquete) = tamanio_total;
    return paquete;
};

char* pack_FIN_LISTA(payload_FIN_LISTA payload,int* tamanio_paquete){
    int tamanio_total = sizeof(HEADER_T);
    char* paquete = malloc(tamanio_total);

    int offset = 0;
    int tamanio_envio;
    HEADER_T cabecera = FIN_LISTA;
    tamanio_envio = (sizeof(HEADER_T));
    memcpy(paquete+offset,&cabecera,tamanio_envio);
    offset += tamanio_envio;

    (* tamanio_paquete) = tamanio_total;
    return paquete;
};

char* pack_FIN_COMUNICACION(payload_FIN_COMUNICACION payload,int* tamanio_paquete){
    int tamanio_total = sizeof(HEADER_T);
    char* paquete = malloc(tamanio_total);

    int offset = 0;
    int tamanio_envio;
    HEADER_T cabecera = FIN_COMUNICACION;
    tamanio_envio = (sizeof(HEADER_T));
    memcpy(paquete+offset,&cabecera,tamanio_envio);
    offset += tamanio_envio;

    (* tamanio_paquete) = tamanio_total;
    return paquete;
};

char* pack_PRESENTACION_DATANODE(payload_PRESENTACION_DATANODE payload,int* tamanio_paquete){
    int tamanio_total = sizeof(HEADER_T) + sizeof(uint16_t) + sizeof(uint16_t) + (payload.tamanio_IP_dataNode);
    char* paquete = malloc(tamanio_total);

    int offset = 0;
    int tamanio_envio;
    HEADER_T cabecera = PRESENTACION_DATANODE;
    tamanio_envio = (sizeof(HEADER_T));
    memcpy(paquete+offset,&cabecera,tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(uint16_t);
    memcpy(paquete+offset,&(payload.PUERTO_dataNode),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(uint16_t);
    memcpy(paquete+offset,&(payload.tamanio_IP_dataNode),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = (payload.tamanio_IP_dataNode);
    memcpy(paquete+offset,payload.IP_dataNode,tamanio_envio);
    offset += tamanio_envio;

    (* tamanio_paquete) = tamanio_total;
    return paquete;
};

