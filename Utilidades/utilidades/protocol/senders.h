
#ifndef UTILIDADES_PROTOCOL_SENDERS_H_
#define UTILIDADES_PROTOCOL_SENDERS_H_

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "types.h"

void send_SOLICITUD_JOB(int socket  , char* nombreArchivo);
void send_ORDEN_TRANSFORMACION(int socket  , int bloque , int bytesocupados , char* nombreArchivoTemporal);
void send_ORDEN_REDUCCIONLOCAL(int socket  , char* nombreTemporal_Transformacion , char* nombreTemporal_ReduccionLocal);
void send_ORDEN_REDUCCIONGLOBAL(int socket  , int PUERTO_Nodo , char* IP_Nodo , char* nombreTemporal_ReduccionLocal , char* nombreTemporal_ReduccionGlobal , int encargado);
void send_ORDEN_ALMACENAMIENTO(int socket  , char* rutaAlmacenamiento , char* nombreAlamcenamiento , char* nombreTemporal_ReduccionGlobal);
void send_INFO_TRANSFORMACION(int socket  , int PUERTO_Worker , char* IP_Worker , int bloque , int bytesocupados , char* nombreArchivoTemporal , int ID_Nodo);
void send_INFO_REDUCCIONLOCAL(int socket  , int PUERTO_Worker , char* IP_Worker , char* nombreTemporal_Transformacion , char* nombreTemporal_ReduccionLocal , int ID_Nodo);
void send_INFO_REDUCCIONGLOBAL(int socket  , int PUERTO_Worker , char* IP_Worker , char* nombreTemporal_ReduccionLocal , char* nombreTemporal_ReduccionGlobal , int encargado , int ID_Nodo);
void send_INFO_ALMACENAMIENTO(int socket  , int PUERTO_Worker , char* IP_Worker , char* nombreTemporal_ReduccionGlobal , int ID_Nodo);
void send_PETICION_NODO(int socket  , char* nombreArchivo);
void send_NODO(int socket  , int PUERTO_Nodo , char* IP_Nodo , char* nombreNodo);
void send_FIN_LISTA(int socket );
void send_FIN_COMUNICACION(int socket );
void send_ARCHIVO(int socket , int archivo_fd);
void send_BLOQUE(int socket, int tamanio_bloque, char* bloque, int id_bloque);
void send_PRESENTACION_DATANODE(int socket  , int id_dataNode , int cantidad_bloques , char* ipDatanode);
void send_JOB(int socket  , int id_master);
void send_RESPUESTA_MASTER(int socket  , int id_master , int id_nodo , int bloque , int estado);
void send_EXITO_OPERACION(int socket );
void send_FRACASO_OPERACION(int socket );
void send_SCRIPT(int socket  , char* contenido);
void send_PETICION_BLOQUE(int socket  , int numero_bloque , int tam_bloque);
void send_UBICACION_BLOQUE(int socket  , char* ip , int puerto , int numero_nodo , int bloque_nodo , int bloque_archivo , int copia , int tam_bloque);
void send_TEMPORAL(int socket  , char* contenido);
void send_PETICION_TEMPORAL(int socket  , char* nombre);
void send_RECHAZO_CONEXION(int socket );
void send_ALMACENAR_ARCHIVO(int socket, int tamanio_bloque, char* bloque, char* pathDestino,char* nombre,char* tipo);

#endif /* UTILIDADES_PROTOCOL_SENDER_H_ */
