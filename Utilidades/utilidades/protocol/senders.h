/*
 * senders.h
 * Generado automaticamente por ProtoCool
*/

#ifndef UTILIDADES_PROTOCOL_SENDERS_H_
#define UTILIDADES_PROTOCOL_SENDERS_H_

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "types.h"

void send_SOLICITUD_PROCESAMIENTO(int socket  , char* nombreArchivo);
void send_SOLICITUD_REDUCCIONLOCAL(int socket );
void send_SOLICITUD_REDUCCIONGLOBAL(int socket );
void send_SOLICITUD_ALMACENAMIENTO(int socket );
void send_ORDEN_TRANSFORMACION(int socket  , uint16_t bloque , uint16_t bytesocupados , char* nombreArchivoTemporal);
void send_ORDEN_REDUCCIONLOCAL(int socket  , char* nombreTemporal_Transformacion , char* nombreTemporal_ReduccionLocal);
void send_ORDEN_REDUCCIONGLOBAL(int socket  , uint16_t PUERTO_Nodo , char* IP_Nodo , char* nombreTemporal_ReduccionLocal , char* nombreTemporal_ReduccionLocal , char* nombreTemporal_ReduccionGlobal , uint16_t encargado);
void send_ORDEN_ALMACENAMIENTO(int socket  , char* nombreTemporal_ReduccionGlobal);
void send_INFO_TRANSFORMACION(int socket  , uint16_t PUERTO_Worker , uint16_t bloque , uint16_t bytesocupados , char* nombreArchivoTemporal);
void send_INFO_REDUCCIONLOCAL(int socket  , uint16_t PUERTO_Worker , char* nombreTemporal_Transformacion , char* nombreTemporal_ReduccionLocal);
void send_INFO_REDUCCIONGLOBAL(int socket  , uint16_t PUERTO_Worker , char* IP_Nodo , char* nombreTemporal_ReduccionLocal , char* nombreTemporal_ReduccionLocal , char* nombreTemporal_ReduccionGlobal , uint16_t encargado);
void send_INFO_ALMACENAMIENTO(int socket  , uint16_t PUERTO_Worker , char* nombreTemporal_ReduccionGlobal);
void send_FIN_LISTA(int socket );
void send_FIN_COMUNICACION(int socket );

#endif /* UTILIDADES_PROTOCOL_SENDER_H_ */
