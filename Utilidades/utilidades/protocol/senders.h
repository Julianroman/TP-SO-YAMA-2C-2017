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

void send_SOLICITUD_JOB(int socket  , char* nombreArchivo);
void send_ORDEN_TRANSFORMACION(int socket  , uint16_t bloque , uint32_t bytesocupados , char* nombreArchivoTemporal);
void send_ORDEN_REDUCCIONLOCAL(int socket  , char* nombreTemporal_Transformacion , char* nombreTemporal_ReduccionLocal);
void send_ORDEN_REDUCCIONGLOBAL(int socket  , uint16_t PUERTO_Nodo , char* IP_Nodo , char* nombreTemporal_ReduccionLocal , char* nombreTemporal_ReduccionGlobal , uint16_t encargado);
void send_ORDEN_ALMACENAMIENTO(int socket  , char* nombreTemporal_ReduccionGlobal);
void send_INFO_TRANSFORMACION(int socket  , uint16_t PUERTO_Worker , char* IP_Worker , uint16_t bloque , uint32_t bytesocupados , char* nombreArchivoTemporal);
void send_INFO_REDUCCIONLOCAL(int socket  , uint16_t PUERTO_Worker , char* IP_Worker , char* nombreTemporal_Transformacion , char* nombreTemporal_ReduccionLocal);
void send_INFO_REDUCCIONGLOBAL(int socket  , uint16_t PUERTO_Worker , char* IP_Worker , char* nombreTemporal_ReduccionLocal , char* nombreTemporal_ReduccionGlobal , uint16_t encargado);
void send_INFO_ALMACENAMIENTO(int socket  , uint16_t PUERTO_Worker , char* IP_Worker , char* nombreTemporal_ReduccionGlobal);
void send_PETICION_NODO(int socket  , char* nombreArchivo);
void send_NODO(int socket  , uint16_t PUERTO_Nodo , char* IP_Nodo , char* nombreNodo);
void send_FIN_LISTA(int socket );
void send_FIN_COMUNICACION(int socket );
void send_ARCHIVO(int socket , int archivo_fd);
void send_BLOQUE(int socket, uint64_t tamanio_bloque, char* bloque, uint32_t id_bloque);
void send_PRESENTACION_DATANODE(int socket  , uint16_t PUERTO_dataNode , char* IP_dataNode);
void send_JOB(int socket  , uint16_t id_master , uint16_t cantidad_operaciones);
void send_RESPUESTA_MASTER(int socket  , uint16_t id_master , uint16_t id_nodo , uint16_t bloque , uint16_t estado);
void send_EXITO_OPERACION(int socket );
void send_FRACASO_OPERACION(int socket );

#endif /* UTILIDADES_PROTOCOL_SENDER_H_ */
