/*
 * types.h
 * Generado automaticamente por ProtocolMaker
*/

#ifndef UTILIDADES_PROTOCOL_TYPES_H_
#define UTILIDADES_PROTOCOL_TYPES_H_

#include <stdint.h>

typedef enum {SOLICITUDPROCESAMIENTO}HEADER_T;

typedef struct { 
    uint16_t tamanio_nombreArchivo; 
    char* nombreArchivo; 
}payload_SOLICITUDPROCESAMIENTO;


#endif /* UTILIDADES_PROTOCOL_TYPES_H_ */
