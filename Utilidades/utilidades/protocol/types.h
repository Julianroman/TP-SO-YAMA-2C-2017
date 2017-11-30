
#ifndef UTILIDADES_PROTOCOL_TYPES_H_
#define UTILIDADES_PROTOCOL_TYPES_H_

#include <stdint.h>

typedef enum {SOLICITUD_JOB, ORDEN_TRANSFORMACION, ORDEN_REDUCCIONLOCAL, ORDEN_REDUCCIONGLOBAL, ORDEN_ALMACENAMIENTO, INFO_TRANSFORMACION, INFO_REDUCCIONLOCAL, INFO_REDUCCIONGLOBAL, INFO_ALMACENAMIENTO, PETICION_NODO, NODO, FIN_LISTA, FIN_COMUNICACION, ARCHIVO, BLOQUE, PRESENTACION_DATANODE, JOB, RESPUESTA_MASTER, EXITO_OPERACION, FRACASO_OPERACION, SCRIPT, PETICION_BLOQUE, UBICACION_BLOQUE, TEMPORAL, PETICION_TEMPORAL}HEADER_T;

typedef struct { 
    int tamanio_nombreArchivo; 
    char* nombreArchivo; 
}payload_SOLICITUD_JOB;

typedef struct { 
    int bloque; 
    int bytesocupados; 
    int tamanio_nombreArchivoTemporal; 
    char* nombreArchivoTemporal; 
}payload_ORDEN_TRANSFORMACION;

typedef struct { 
    int tamanio_nombreTemporal_Transformacion; 
    char* nombreTemporal_Transformacion; 
    int tamanio_nombreTemporal_ReduccionLocal; 
    char* nombreTemporal_ReduccionLocal; 
}payload_ORDEN_REDUCCIONLOCAL;

typedef struct { 
    int PUERTO_Nodo; 
    int tamanio_IP_Nodo; 
    char* IP_Nodo; 
    int tamanio_nombreTemporal_ReduccionLocal; 
    char* nombreTemporal_ReduccionLocal; 
    int tamanio_nombreTemporal_ReduccionGlobal; 
    char* nombreTemporal_ReduccionGlobal; 
    int encargado; 
}payload_ORDEN_REDUCCIONGLOBAL;

typedef struct { 
    int tamanio_nombreTemporal_ReduccionGlobal; 
    char* nombreTemporal_ReduccionGlobal; 
}payload_ORDEN_ALMACENAMIENTO;

typedef struct { 
    int PUERTO_Worker; 
    int tamanio_IP_Worker; 
    char* IP_Worker; 
    int bloque; 
    int bytesocupados; 
    int tamanio_nombreArchivoTemporal; 
    char* nombreArchivoTemporal; 
}payload_INFO_TRANSFORMACION;

typedef struct { 
    int PUERTO_Worker; 
    int tamanio_IP_Worker; 
    char* IP_Worker; 
    int tamanio_nombreTemporal_Transformacion; 
    char* nombreTemporal_Transformacion; 
    int tamanio_nombreTemporal_ReduccionLocal; 
    char* nombreTemporal_ReduccionLocal; 
}payload_INFO_REDUCCIONLOCAL;

typedef struct { 
    int PUERTO_Worker; 
    int tamanio_IP_Worker; 
    char* IP_Worker; 
    int tamanio_nombreTemporal_ReduccionLocal; 
    char* nombreTemporal_ReduccionLocal; 
    int tamanio_nombreTemporal_ReduccionGlobal; 
    char* nombreTemporal_ReduccionGlobal; 
    int encargado; 
}payload_INFO_REDUCCIONGLOBAL;

typedef struct { 
    int PUERTO_Worker; 
    int tamanio_IP_Worker; 
    char* IP_Worker; 
    int tamanio_nombreTemporal_ReduccionGlobal; 
    char* nombreTemporal_ReduccionGlobal; 
}payload_INFO_ALMACENAMIENTO;

typedef struct { 
    int tamanio_nombreArchivo; 
    char* nombreArchivo; 
}payload_PETICION_NODO;

typedef struct { 
    int PUERTO_Nodo; 
    int tamanio_IP_Nodo; 
    char* IP_Nodo; 
    int tamanio_nombreNodo; 
    char* nombreNodo; 
}payload_NODO;

typedef struct { 
}payload_FIN_LISTA;

typedef struct { 
}payload_FIN_COMUNICACION;

typedef struct { 
    int tamanio_archivo; 
    char* archivo; 
}payload_ARCHIVO;

typedef struct { 
    int tamanio_bloque; 
    char* contenido; 
    int numero_bloque; 
}payload_BLOQUE;

typedef struct { 
    int pid; 
    int id_dataNode; 
    int cantidad_bloques; 
}payload_PRESENTACION_DATANODE;

typedef struct { 
    int id_master; 
}payload_JOB;

typedef struct { 
    int id_master; 
    int id_nodo; 
    int bloque; 
    int estado; 
}payload_RESPUESTA_MASTER;

typedef struct { 
}payload_EXITO_OPERACION;

typedef struct { 
}payload_FRACASO_OPERACION;

typedef struct { 
    int tamanio_contenido; 
    char* contenido; 
}payload_SCRIPT;

typedef struct { 
    int numero_bloque; 
    int tam_bloque; 
}payload_PETICION_BLOQUE;

typedef struct { 
    int numero_nodo; 
    int bloque_nodo; 
    int bloque_archivo; 
    int copia; 
}payload_UBICACION_BLOQUE;

typedef struct { 
    int tamanio_contenido; 
    char* contenido; 
}payload_TEMPORAL;

typedef struct { 
    int tamanio_nombre; 
    char* nombre; 
}payload_PETICION_TEMPORAL;


#endif /* UTILIDADES_PROTOCOL_TYPES_H_ */
