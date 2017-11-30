
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "types.h"

char* pack_SOLICITUD_JOB(payload_SOLICITUD_JOB payload,int* tamanio_paquete){
    int tamanio_total = sizeof(HEADER_T) + sizeof(int) + (payload.tamanio_nombreArchivo);
    char* paquete = malloc(tamanio_total);

    int offset = 0;
    int tamanio_envio;
    HEADER_T cabecera = SOLICITUD_JOB;
    tamanio_envio = (sizeof(HEADER_T));
    memcpy(paquete+offset,&cabecera,tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(int);
    memcpy(paquete+offset,&(payload.tamanio_nombreArchivo),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = (payload.tamanio_nombreArchivo);
    memcpy(paquete+offset,payload.nombreArchivo,tamanio_envio);
    offset += tamanio_envio;

    (* tamanio_paquete) = tamanio_total;
    return paquete;
};

char* pack_ORDEN_TRANSFORMACION(payload_ORDEN_TRANSFORMACION payload,int* tamanio_paquete){
    int tamanio_total = sizeof(HEADER_T) + sizeof(int) + sizeof(int) + sizeof(int) + (payload.tamanio_nombreArchivoTemporal);
    char* paquete = malloc(tamanio_total);

    int offset = 0;
    int tamanio_envio;
    HEADER_T cabecera = ORDEN_TRANSFORMACION;
    tamanio_envio = (sizeof(HEADER_T));
    memcpy(paquete+offset,&cabecera,tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(int);
    memcpy(paquete+offset,&(payload.bloque),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(int);
    memcpy(paquete+offset,&(payload.bytesocupados),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(int);
    memcpy(paquete+offset,&(payload.tamanio_nombreArchivoTemporal),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = (payload.tamanio_nombreArchivoTemporal);
    memcpy(paquete+offset,payload.nombreArchivoTemporal,tamanio_envio);
    offset += tamanio_envio;

    (* tamanio_paquete) = tamanio_total;
    return paquete;
};

char* pack_ORDEN_REDUCCIONLOCAL(payload_ORDEN_REDUCCIONLOCAL payload,int* tamanio_paquete){
    int tamanio_total = sizeof(HEADER_T) + sizeof(int) + (payload.tamanio_nombreTemporal_Transformacion) + sizeof(int) + (payload.tamanio_nombreTemporal_ReduccionLocal);
    char* paquete = malloc(tamanio_total);

    int offset = 0;
    int tamanio_envio;
    HEADER_T cabecera = ORDEN_REDUCCIONLOCAL;
    tamanio_envio = (sizeof(HEADER_T));
    memcpy(paquete+offset,&cabecera,tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(int);
    memcpy(paquete+offset,&(payload.tamanio_nombreTemporal_Transformacion),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = (payload.tamanio_nombreTemporal_Transformacion);
    memcpy(paquete+offset,payload.nombreTemporal_Transformacion,tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(int);
    memcpy(paquete+offset,&(payload.tamanio_nombreTemporal_ReduccionLocal),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = (payload.tamanio_nombreTemporal_ReduccionLocal);
    memcpy(paquete+offset,payload.nombreTemporal_ReduccionLocal,tamanio_envio);
    offset += tamanio_envio;

    (* tamanio_paquete) = tamanio_total;
    return paquete;
};

char* pack_ORDEN_REDUCCIONGLOBAL(payload_ORDEN_REDUCCIONGLOBAL payload,int* tamanio_paquete){
    int tamanio_total = sizeof(HEADER_T) + sizeof(int) + sizeof(int) + (payload.tamanio_IP_Nodo) + sizeof(int) + (payload.tamanio_nombreTemporal_ReduccionLocal) + sizeof(int) + (payload.tamanio_nombreTemporal_ReduccionGlobal) + sizeof(int);
    char* paquete = malloc(tamanio_total);

    int offset = 0;
    int tamanio_envio;
    HEADER_T cabecera = ORDEN_REDUCCIONGLOBAL;
    tamanio_envio = (sizeof(HEADER_T));
    memcpy(paquete+offset,&cabecera,tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(int);
    memcpy(paquete+offset,&(payload.PUERTO_Nodo),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(int);
    memcpy(paquete+offset,&(payload.tamanio_IP_Nodo),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = (payload.tamanio_IP_Nodo);
    memcpy(paquete+offset,payload.IP_Nodo,tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(int);
    memcpy(paquete+offset,&(payload.tamanio_nombreTemporal_ReduccionLocal),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = (payload.tamanio_nombreTemporal_ReduccionLocal);
    memcpy(paquete+offset,payload.nombreTemporal_ReduccionLocal,tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(int);
    memcpy(paquete+offset,&(payload.tamanio_nombreTemporal_ReduccionGlobal),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = (payload.tamanio_nombreTemporal_ReduccionGlobal);
    memcpy(paquete+offset,payload.nombreTemporal_ReduccionGlobal,tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(int);
    memcpy(paquete+offset,&(payload.encargado),tamanio_envio);
    offset += tamanio_envio;

    (* tamanio_paquete) = tamanio_total;
    return paquete;
};

char* pack_ORDEN_ALMACENAMIENTO(payload_ORDEN_ALMACENAMIENTO payload,int* tamanio_paquete){
    int tamanio_total = sizeof(HEADER_T) + sizeof(int) + (payload.tamanio_nombreTemporal_ReduccionGlobal);
    char* paquete = malloc(tamanio_total);

    int offset = 0;
    int tamanio_envio;
    HEADER_T cabecera = ORDEN_ALMACENAMIENTO;
    tamanio_envio = (sizeof(HEADER_T));
    memcpy(paquete+offset,&cabecera,tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(int);
    memcpy(paquete+offset,&(payload.tamanio_nombreTemporal_ReduccionGlobal),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = (payload.tamanio_nombreTemporal_ReduccionGlobal);
    memcpy(paquete+offset,payload.nombreTemporal_ReduccionGlobal,tamanio_envio);
    offset += tamanio_envio;

    (* tamanio_paquete) = tamanio_total;
    return paquete;
};

char* pack_INFO_TRANSFORMACION(payload_INFO_TRANSFORMACION payload,int* tamanio_paquete){
    int tamanio_total = sizeof(HEADER_T) + sizeof(int) + sizeof(int) + (payload.tamanio_IP_Worker) + sizeof(int) + sizeof(int) + sizeof(int) + (payload.tamanio_nombreArchivoTemporal);
    char* paquete = malloc(tamanio_total);

    int offset = 0;
    int tamanio_envio;
    HEADER_T cabecera = INFO_TRANSFORMACION;
    tamanio_envio = (sizeof(HEADER_T));
    memcpy(paquete+offset,&cabecera,tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(int);
    memcpy(paquete+offset,&(payload.PUERTO_Worker),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(int);
    memcpy(paquete+offset,&(payload.tamanio_IP_Worker),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = (payload.tamanio_IP_Worker);
    memcpy(paquete+offset,payload.IP_Worker,tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(int);
    memcpy(paquete+offset,&(payload.bloque),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(int);
    memcpy(paquete+offset,&(payload.bytesocupados),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(int);
    memcpy(paquete+offset,&(payload.tamanio_nombreArchivoTemporal),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = (payload.tamanio_nombreArchivoTemporal);
    memcpy(paquete+offset,payload.nombreArchivoTemporal,tamanio_envio);
    offset += tamanio_envio;

    (* tamanio_paquete) = tamanio_total;
    return paquete;
};

char* pack_INFO_REDUCCIONLOCAL(payload_INFO_REDUCCIONLOCAL payload,int* tamanio_paquete){
    int tamanio_total = sizeof(HEADER_T) + sizeof(int) + sizeof(int) + (payload.tamanio_IP_Worker) + sizeof(int) + (payload.tamanio_nombreTemporal_Transformacion) + sizeof(int) + (payload.tamanio_nombreTemporal_ReduccionLocal);
    char* paquete = malloc(tamanio_total);

    int offset = 0;
    int tamanio_envio;
    HEADER_T cabecera = INFO_REDUCCIONLOCAL;
    tamanio_envio = (sizeof(HEADER_T));
    memcpy(paquete+offset,&cabecera,tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(int);
    memcpy(paquete+offset,&(payload.PUERTO_Worker),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(int);
    memcpy(paquete+offset,&(payload.tamanio_IP_Worker),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = (payload.tamanio_IP_Worker);
    memcpy(paquete+offset,payload.IP_Worker,tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(int);
    memcpy(paquete+offset,&(payload.tamanio_nombreTemporal_Transformacion),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = (payload.tamanio_nombreTemporal_Transformacion);
    memcpy(paquete+offset,payload.nombreTemporal_Transformacion,tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(int);
    memcpy(paquete+offset,&(payload.tamanio_nombreTemporal_ReduccionLocal),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = (payload.tamanio_nombreTemporal_ReduccionLocal);
    memcpy(paquete+offset,payload.nombreTemporal_ReduccionLocal,tamanio_envio);
    offset += tamanio_envio;

    (* tamanio_paquete) = tamanio_total;
    return paquete;
};

char* pack_INFO_REDUCCIONGLOBAL(payload_INFO_REDUCCIONGLOBAL payload,int* tamanio_paquete){
    int tamanio_total = sizeof(HEADER_T) + sizeof(int) + sizeof(int) + (payload.tamanio_IP_Worker) + sizeof(int) + (payload.tamanio_nombreTemporal_ReduccionLocal) + sizeof(int) + (payload.tamanio_nombreTemporal_ReduccionGlobal) + sizeof(int);
    char* paquete = malloc(tamanio_total);

    int offset = 0;
    int tamanio_envio;
    HEADER_T cabecera = INFO_REDUCCIONGLOBAL;
    tamanio_envio = (sizeof(HEADER_T));
    memcpy(paquete+offset,&cabecera,tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(int);
    memcpy(paquete+offset,&(payload.PUERTO_Worker),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(int);
    memcpy(paquete+offset,&(payload.tamanio_IP_Worker),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = (payload.tamanio_IP_Worker);
    memcpy(paquete+offset,payload.IP_Worker,tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(int);
    memcpy(paquete+offset,&(payload.tamanio_nombreTemporal_ReduccionLocal),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = (payload.tamanio_nombreTemporal_ReduccionLocal);
    memcpy(paquete+offset,payload.nombreTemporal_ReduccionLocal,tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(int);
    memcpy(paquete+offset,&(payload.tamanio_nombreTemporal_ReduccionGlobal),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = (payload.tamanio_nombreTemporal_ReduccionGlobal);
    memcpy(paquete+offset,payload.nombreTemporal_ReduccionGlobal,tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(int);
    memcpy(paquete+offset,&(payload.encargado),tamanio_envio);
    offset += tamanio_envio;

    (* tamanio_paquete) = tamanio_total;
    return paquete;
};

char* pack_INFO_ALMACENAMIENTO(payload_INFO_ALMACENAMIENTO payload,int* tamanio_paquete){
    int tamanio_total = sizeof(HEADER_T) + sizeof(int) + sizeof(int) + (payload.tamanio_IP_Worker) + sizeof(int) + (payload.tamanio_nombreTemporal_ReduccionGlobal);
    char* paquete = malloc(tamanio_total);

    int offset = 0;
    int tamanio_envio;
    HEADER_T cabecera = INFO_ALMACENAMIENTO;
    tamanio_envio = (sizeof(HEADER_T));
    memcpy(paquete+offset,&cabecera,tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(int);
    memcpy(paquete+offset,&(payload.PUERTO_Worker),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(int);
    memcpy(paquete+offset,&(payload.tamanio_IP_Worker),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = (payload.tamanio_IP_Worker);
    memcpy(paquete+offset,payload.IP_Worker,tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(int);
    memcpy(paquete+offset,&(payload.tamanio_nombreTemporal_ReduccionGlobal),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = (payload.tamanio_nombreTemporal_ReduccionGlobal);
    memcpy(paquete+offset,payload.nombreTemporal_ReduccionGlobal,tamanio_envio);
    offset += tamanio_envio;

    (* tamanio_paquete) = tamanio_total;
    return paquete;
};

char* pack_PETICION_NODO(payload_PETICION_NODO payload,int* tamanio_paquete){
    int tamanio_total = sizeof(HEADER_T) + sizeof(int) + (payload.tamanio_nombreArchivo);
    char* paquete = malloc(tamanio_total);

    int offset = 0;
    int tamanio_envio;
    HEADER_T cabecera = PETICION_NODO;
    tamanio_envio = (sizeof(HEADER_T));
    memcpy(paquete+offset,&cabecera,tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(int);
    memcpy(paquete+offset,&(payload.tamanio_nombreArchivo),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = (payload.tamanio_nombreArchivo);
    memcpy(paquete+offset,payload.nombreArchivo,tamanio_envio);
    offset += tamanio_envio;

    (* tamanio_paquete) = tamanio_total;
    return paquete;
};

char* pack_NODO(payload_NODO payload,int* tamanio_paquete){
    int tamanio_total = sizeof(HEADER_T) + sizeof(int) + sizeof(int) + (payload.tamanio_IP_Nodo) + sizeof(int) + (payload.tamanio_nombreNodo);
    char* paquete = malloc(tamanio_total);

    int offset = 0;
    int tamanio_envio;
    HEADER_T cabecera = NODO;
    tamanio_envio = (sizeof(HEADER_T));
    memcpy(paquete+offset,&cabecera,tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(int);
    memcpy(paquete+offset,&(payload.PUERTO_Nodo),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(int);
    memcpy(paquete+offset,&(payload.tamanio_IP_Nodo),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = (payload.tamanio_IP_Nodo);
    memcpy(paquete+offset,payload.IP_Nodo,tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(int);
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
    int tamanio_total = sizeof(HEADER_T) + sizeof(int) + sizeof(int) + sizeof(int);
    char* paquete = malloc(tamanio_total);

    int offset = 0;
    int tamanio_envio;
    HEADER_T cabecera = PRESENTACION_DATANODE;
    tamanio_envio = (sizeof(HEADER_T));
    memcpy(paquete+offset,&cabecera,tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(int);
    memcpy(paquete+offset,&(payload.pid),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(int);
    memcpy(paquete+offset,&(payload.id_dataNode),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(int);
    memcpy(paquete+offset,&(payload.cantidad_bloques),tamanio_envio);
    offset += tamanio_envio;

    (* tamanio_paquete) = tamanio_total;
    return paquete;
};

char* pack_JOB(payload_JOB payload,int* tamanio_paquete){
    int tamanio_total = sizeof(HEADER_T) + sizeof(int);
    char* paquete = malloc(tamanio_total);

    int offset = 0;
    int tamanio_envio;
    HEADER_T cabecera = JOB;
    tamanio_envio = (sizeof(HEADER_T));
    memcpy(paquete+offset,&cabecera,tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(int);
    memcpy(paquete+offset,&(payload.id_master),tamanio_envio);
    offset += tamanio_envio;

    (* tamanio_paquete) = tamanio_total;
    return paquete;
};

char* pack_RESPUESTA_MASTER(payload_RESPUESTA_MASTER payload,int* tamanio_paquete){
    int tamanio_total = sizeof(HEADER_T) + sizeof(int) + sizeof(int) + sizeof(int) + sizeof(int);
    char* paquete = malloc(tamanio_total);

    int offset = 0;
    int tamanio_envio;
    HEADER_T cabecera = RESPUESTA_MASTER;
    tamanio_envio = (sizeof(HEADER_T));
    memcpy(paquete+offset,&cabecera,tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(int);
    memcpy(paquete+offset,&(payload.id_master),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(int);
    memcpy(paquete+offset,&(payload.id_nodo),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(int);
    memcpy(paquete+offset,&(payload.bloque),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(int);
    memcpy(paquete+offset,&(payload.estado),tamanio_envio);
    offset += tamanio_envio;

    (* tamanio_paquete) = tamanio_total;
    return paquete;
};

char* pack_EXITO_OPERACION(payload_EXITO_OPERACION payload,int* tamanio_paquete){
    int tamanio_total = sizeof(HEADER_T);
    char* paquete = malloc(tamanio_total);

    int offset = 0;
    int tamanio_envio;
    HEADER_T cabecera = EXITO_OPERACION;
    tamanio_envio = (sizeof(HEADER_T));
    memcpy(paquete+offset,&cabecera,tamanio_envio);
    offset += tamanio_envio;

    (* tamanio_paquete) = tamanio_total;
    return paquete;
};

char* pack_FRACASO_OPERACION(payload_FRACASO_OPERACION payload,int* tamanio_paquete){
    int tamanio_total = sizeof(HEADER_T);
    char* paquete = malloc(tamanio_total);

    int offset = 0;
    int tamanio_envio;
    HEADER_T cabecera = FRACASO_OPERACION;
    tamanio_envio = (sizeof(HEADER_T));
    memcpy(paquete+offset,&cabecera,tamanio_envio);
    offset += tamanio_envio;

    (* tamanio_paquete) = tamanio_total;
    return paquete;
};

char* pack_SCRIPT(payload_SCRIPT payload,int* tamanio_paquete){
    int tamanio_total = sizeof(HEADER_T) + sizeof(int) + (payload.tamanio_contenido);
    char* paquete = malloc(tamanio_total);

    int offset = 0;
    int tamanio_envio;
    HEADER_T cabecera = SCRIPT;
    tamanio_envio = (sizeof(HEADER_T));
    memcpy(paquete+offset,&cabecera,tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(int);
    memcpy(paquete+offset,&(payload.tamanio_contenido),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = (payload.tamanio_contenido);
    memcpy(paquete+offset,payload.contenido,tamanio_envio);
    offset += tamanio_envio;

    (* tamanio_paquete) = tamanio_total;
    return paquete;
};

char* pack_PETICION_BLOQUE(payload_PETICION_BLOQUE payload,int* tamanio_paquete){
    int tamanio_total = sizeof(HEADER_T) + sizeof(int) + sizeof(int);
    char* paquete = malloc(tamanio_total);

    int offset = 0;
    int tamanio_envio;
    HEADER_T cabecera = PETICION_BLOQUE;
    tamanio_envio = (sizeof(HEADER_T));
    memcpy(paquete+offset,&cabecera,tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(int);
    memcpy(paquete+offset,&(payload.numero_bloque),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(int);
    memcpy(paquete+offset,&(payload.tam_bloque),tamanio_envio);
    offset += tamanio_envio;

    (* tamanio_paquete) = tamanio_total;
    return paquete;
};

char* pack_UBICACION_BLOQUE(payload_UBICACION_BLOQUE payload,int* tamanio_paquete){
    int tamanio_total = sizeof(HEADER_T) + sizeof(int) + sizeof(int) + sizeof(int) + sizeof(int);
    char* paquete = malloc(tamanio_total);

    int offset = 0;
    int tamanio_envio;
    HEADER_T cabecera = UBICACION_BLOQUE;
    tamanio_envio = (sizeof(HEADER_T));
    memcpy(paquete+offset,&cabecera,tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(int);
    memcpy(paquete+offset,&(payload.numero_nodo),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(int);
    memcpy(paquete+offset,&(payload.bloque_nodo),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(int);
    memcpy(paquete+offset,&(payload.bloque_archivo),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(int);
    memcpy(paquete+offset,&(payload.copia),tamanio_envio);
    offset += tamanio_envio;

    (* tamanio_paquete) = tamanio_total;
    return paquete;
};

char* pack_ARCHIVO(payload_ARCHIVO payload,int* tamanio_paquete){
    return NULL;
};
char* pack_BLOQUE(payload_BLOQUE payload,int* tamanio_paquete){
    int tamanio_total = sizeof(HEADER_T) + sizeof(int) + payload.tamanio_bloque + sizeof(int);
    char* paquete = malloc(tamanio_total);

    int offset = 0;
    int tamanio_envio;
    HEADER_T cabecera = BLOQUE;
    tamanio_envio = (sizeof(HEADER_T));
    memcpy(paquete+offset,&cabecera,tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(int);
    memcpy(paquete+offset,&(payload.tamanio_bloque),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = (payload.tamanio_bloque);
    memcpy(paquete+offset,(payload.contenido),tamanio_envio);
    offset += tamanio_envio;

    tamanio_envio = sizeof(int);
    memcpy(paquete+offset,&(payload.numero_bloque),tamanio_envio);
    offset += tamanio_envio;

    (* tamanio_paquete) = tamanio_total;
    return paquete;
};
