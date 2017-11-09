/*
 * senders.c
 * Generado automaticamente por ProtoCool
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "types.h"
#include "packers.h"
#include "enviar_paquete.h"

void send_SOLICITUD_JOB(int socket , char* nombreArchivo){
    payload_SOLICITUD_JOB payload;
    payload.tamanio_nombreArchivo = (strlen(nombreArchivo)+1)*sizeof(char);
    payload.nombreArchivo = nombreArchivo; 

    int tamanio_paquete;
    char* paquete = pack_SOLICITUD_JOB(payload,&tamanio_paquete);
    enviar_paquete(socket,paquete,tamanio_paquete);
    free(paquete);
};

void send_ORDEN_TRANSFORMACION(int socket , uint16_t bloque , uint32_t bytesocupados , char* nombreArchivoTemporal){
    payload_ORDEN_TRANSFORMACION payload;
    payload.bloque = bloque; 
    payload.bytesocupados = bytesocupados; 
    payload.tamanio_nombreArchivoTemporal = (strlen(nombreArchivoTemporal)+1)*sizeof(char);
    payload.nombreArchivoTemporal = nombreArchivoTemporal; 

    int tamanio_paquete;
    char* paquete = pack_ORDEN_TRANSFORMACION(payload,&tamanio_paquete);
    enviar_paquete(socket,paquete,tamanio_paquete);
    free(paquete);
};

void send_ORDEN_REDUCCIONLOCAL(int socket , char* nombreTemporal_Transformacion , char* nombreTemporal_ReduccionLocal){
    payload_ORDEN_REDUCCIONLOCAL payload;
    payload.tamanio_nombreTemporal_Transformacion = (strlen(nombreTemporal_Transformacion)+1)*sizeof(char);
    payload.nombreTemporal_Transformacion = nombreTemporal_Transformacion; 
    payload.tamanio_nombreTemporal_ReduccionLocal = (strlen(nombreTemporal_ReduccionLocal)+1)*sizeof(char);
    payload.nombreTemporal_ReduccionLocal = nombreTemporal_ReduccionLocal; 

    int tamanio_paquete;
    char* paquete = pack_ORDEN_REDUCCIONLOCAL(payload,&tamanio_paquete);
    enviar_paquete(socket,paquete,tamanio_paquete);
    free(paquete);
};

void send_ORDEN_REDUCCIONGLOBAL(int socket , uint16_t PUERTO_Nodo , char* IP_Nodo , char* nombreTemporal_ReduccionLocal , char* nombreTemporal_ReduccionGlobal , uint16_t encargado){
    payload_ORDEN_REDUCCIONGLOBAL payload;
    payload.PUERTO_Nodo = PUERTO_Nodo; 
    payload.tamanio_IP_Nodo = (strlen(IP_Nodo)+1)*sizeof(char);
    payload.IP_Nodo = IP_Nodo; 
    payload.tamanio_nombreTemporal_ReduccionLocal = (strlen(nombreTemporal_ReduccionLocal)+1)*sizeof(char);
    payload.nombreTemporal_ReduccionLocal = nombreTemporal_ReduccionLocal; 
    payload.tamanio_nombreTemporal_ReduccionGlobal = (strlen(nombreTemporal_ReduccionGlobal)+1)*sizeof(char);
    payload.nombreTemporal_ReduccionGlobal = nombreTemporal_ReduccionGlobal; 
    payload.encargado = encargado; 

    int tamanio_paquete;
    char* paquete = pack_ORDEN_REDUCCIONGLOBAL(payload,&tamanio_paquete);
    enviar_paquete(socket,paquete,tamanio_paquete);
    free(paquete);
};

void send_ORDEN_ALMACENAMIENTO(int socket , char* nombreTemporal_ReduccionGlobal){
    payload_ORDEN_ALMACENAMIENTO payload;
    payload.tamanio_nombreTemporal_ReduccionGlobal = (strlen(nombreTemporal_ReduccionGlobal)+1)*sizeof(char);
    payload.nombreTemporal_ReduccionGlobal = nombreTemporal_ReduccionGlobal; 

    int tamanio_paquete;
    char* paquete = pack_ORDEN_ALMACENAMIENTO(payload,&tamanio_paquete);
    enviar_paquete(socket,paquete,tamanio_paquete);
    free(paquete);
};

void send_INFO_TRANSFORMACION(int socket , uint16_t PUERTO_Worker , char* IP_Worker , uint16_t bloque , uint32_t bytesocupados , char* nombreArchivoTemporal){
    payload_INFO_TRANSFORMACION payload;
    payload.PUERTO_Worker = PUERTO_Worker; 
    payload.tamanio_IP_Worker = (strlen(IP_Worker)+1)*sizeof(char);
    payload.IP_Worker = IP_Worker; 
    payload.bloque = bloque; 
    payload.bytesocupados = bytesocupados; 
    payload.tamanio_nombreArchivoTemporal = (strlen(nombreArchivoTemporal)+1)*sizeof(char);
    payload.nombreArchivoTemporal = nombreArchivoTemporal; 

    int tamanio_paquete;
    char* paquete = pack_INFO_TRANSFORMACION(payload,&tamanio_paquete);
    enviar_paquete(socket,paquete,tamanio_paquete);
    free(paquete);
};

void send_INFO_REDUCCIONLOCAL(int socket , uint16_t PUERTO_Worker , char* IP_Worker , char* nombreTemporal_Transformacion , char* nombreTemporal_ReduccionLocal){
    payload_INFO_REDUCCIONLOCAL payload;
    payload.PUERTO_Worker = PUERTO_Worker; 
    payload.tamanio_IP_Worker = (strlen(IP_Worker)+1)*sizeof(char);
    payload.IP_Worker = IP_Worker; 
    payload.tamanio_nombreTemporal_Transformacion = (strlen(nombreTemporal_Transformacion)+1)*sizeof(char);
    payload.nombreTemporal_Transformacion = nombreTemporal_Transformacion; 
    payload.tamanio_nombreTemporal_ReduccionLocal = (strlen(nombreTemporal_ReduccionLocal)+1)*sizeof(char);
    payload.nombreTemporal_ReduccionLocal = nombreTemporal_ReduccionLocal; 

    int tamanio_paquete;
    char* paquete = pack_INFO_REDUCCIONLOCAL(payload,&tamanio_paquete);
    enviar_paquete(socket,paquete,tamanio_paquete);
    free(paquete);
};

void send_INFO_REDUCCIONGLOBAL(int socket , uint16_t PUERTO_Worker , char* IP_Worker , char* nombreTemporal_ReduccionLocal , char* nombreTemporal_ReduccionGlobal , uint16_t encargado){
    payload_INFO_REDUCCIONGLOBAL payload;
    payload.PUERTO_Worker = PUERTO_Worker; 
    payload.tamanio_IP_Worker = (strlen(IP_Worker)+1)*sizeof(char);
    payload.IP_Worker = IP_Worker; 
    payload.tamanio_nombreTemporal_ReduccionLocal = (strlen(nombreTemporal_ReduccionLocal)+1)*sizeof(char);
    payload.nombreTemporal_ReduccionLocal = nombreTemporal_ReduccionLocal; 
    payload.tamanio_nombreTemporal_ReduccionGlobal = (strlen(nombreTemporal_ReduccionGlobal)+1)*sizeof(char);
    payload.nombreTemporal_ReduccionGlobal = nombreTemporal_ReduccionGlobal; 
    payload.encargado = encargado; 

    int tamanio_paquete;
    char* paquete = pack_INFO_REDUCCIONGLOBAL(payload,&tamanio_paquete);
    enviar_paquete(socket,paquete,tamanio_paquete);
    free(paquete);
};

void send_INFO_ALMACENAMIENTO(int socket , uint16_t PUERTO_Worker , char* IP_Worker , char* nombreTemporal_ReduccionGlobal){
    payload_INFO_ALMACENAMIENTO payload;
    payload.PUERTO_Worker = PUERTO_Worker; 
    payload.tamanio_IP_Worker = (strlen(IP_Worker)+1)*sizeof(char);
    payload.IP_Worker = IP_Worker; 
    payload.tamanio_nombreTemporal_ReduccionGlobal = (strlen(nombreTemporal_ReduccionGlobal)+1)*sizeof(char);
    payload.nombreTemporal_ReduccionGlobal = nombreTemporal_ReduccionGlobal; 

    int tamanio_paquete;
    char* paquete = pack_INFO_ALMACENAMIENTO(payload,&tamanio_paquete);
    enviar_paquete(socket,paquete,tamanio_paquete);
    free(paquete);
};

void send_PETICION_NODO(int socket , char* nombreArchivo){
    payload_PETICION_NODO payload;
    payload.tamanio_nombreArchivo = (strlen(nombreArchivo)+1)*sizeof(char);
    payload.nombreArchivo = nombreArchivo; 

    int tamanio_paquete;
    char* paquete = pack_PETICION_NODO(payload,&tamanio_paquete);
    enviar_paquete(socket,paquete,tamanio_paquete);
    free(paquete);
};

void send_NODO(int socket , uint16_t PUERTO_Nodo , char* IP_Nodo , char* nombreNodo){
    payload_NODO payload;
    payload.PUERTO_Nodo = PUERTO_Nodo; 
    payload.tamanio_IP_Nodo = (strlen(IP_Nodo)+1)*sizeof(char);
    payload.IP_Nodo = IP_Nodo; 
    payload.tamanio_nombreNodo = (strlen(nombreNodo)+1)*sizeof(char);
    payload.nombreNodo = nombreNodo; 

    int tamanio_paquete;
    char* paquete = pack_NODO(payload,&tamanio_paquete);
    enviar_paquete(socket,paquete,tamanio_paquete);
    free(paquete);
};

void send_FIN_LISTA(int socket){
    payload_FIN_LISTA payload;

    int tamanio_paquete;
    char* paquete = pack_FIN_LISTA(payload,&tamanio_paquete);
    enviar_paquete(socket,paquete,tamanio_paquete);
    free(paquete);
};

void send_FIN_COMUNICACION(int socket){
    payload_FIN_COMUNICACION payload;

    int tamanio_paquete;
    char* paquete = pack_FIN_COMUNICACION(payload,&tamanio_paquete);
    enviar_paquete(socket,paquete,tamanio_paquete);
    free(paquete);
};

void send_ARCHIVO(int socket , int archivo_fd){

	struct stat buffer;
	int status = fstat(archivo_fd,&buffer);
	if(status != 0){
		puts("No se pudieron reconocer las estadisticas del ejecutable");
	}
	HEADER_T header = ARCHIVO;
	uint32_t size = buffer.st_size;

	char* paquete = malloc(sizeof(HEADER_T)+sizeof(uint64_t)+size);
	int offset = 0;
	memcpy(paquete+offset,&header,sizeof(HEADER_T));
	offset += sizeof(HEADER_T);
	memcpy(paquete+offset,&size,sizeof(uint64_t));
	offset += sizeof(uint64_t);
	read(archivo_fd,paquete+offset,size);
	enviar_paquete(socket,paquete,size);
	free(paquete);

};
void send_BLOQUE(int socket, uint64_t tamanio_bloque, char* bloque, uint32_t id_bloque){

	HEADER_T header = BLOQUE;

	char* paquete = malloc(sizeof(HEADER_T)+sizeof(uint64_t)+tamanio_bloque+sizeof(uint32_t));

	int offset = 0;

	memcpy(paquete+offset,&header,sizeof(HEADER_T));
	offset += sizeof(HEADER_T);

	memcpy(paquete+offset,&tamanio_bloque,sizeof(uint64_t));
	offset += sizeof(uint64_t);

	memcpy(paquete+offset,bloque,tamanio_bloque);
	offset += tamanio_bloque;

	memcpy(paquete+offset,&id_bloque,sizeof(uint32_t));
	offset += sizeof(uint32_t);

	free(paquete);

};
void send_PRESENTACION_DATANODE(int socket , uint16_t PUERTO_dataNode , char* IP_dataNode , uint16_t pid , uint16_t id_dataNode , uint16_t cantidad_bloques){
    payload_PRESENTACION_DATANODE payload;
    payload.PUERTO_dataNode = PUERTO_dataNode; 
    payload.tamanio_IP_dataNode = (strlen(IP_dataNode)+1)*sizeof(char);
    payload.IP_dataNode = IP_dataNode; 
    payload.pid = pid; 
    payload.id_dataNode = id_dataNode; 
    payload.cantidad_bloques = cantidad_bloques; 

    int tamanio_paquete;
    char* paquete = pack_PRESENTACION_DATANODE(payload,&tamanio_paquete);
    enviar_paquete(socket,paquete,tamanio_paquete);
    free(paquete);
};

void send_JOB(int socket , uint16_t id_master , uint16_t cantidad_operaciones){
    payload_JOB payload;
    payload.id_master = id_master; 
    payload.cantidad_operaciones = cantidad_operaciones; 

    int tamanio_paquete;
    char* paquete = pack_JOB(payload,&tamanio_paquete);
    enviar_paquete(socket,paquete,tamanio_paquete);
    free(paquete);
};

void send_RESPUESTA_MASTER(int socket , uint16_t id_master , uint16_t id_nodo , uint16_t bloque , uint16_t estado){
    payload_RESPUESTA_MASTER payload;
    payload.id_master = id_master; 
    payload.id_nodo = id_nodo; 
    payload.bloque = bloque; 
    payload.estado = estado; 

    int tamanio_paquete;
    char* paquete = pack_RESPUESTA_MASTER(payload,&tamanio_paquete);
    enviar_paquete(socket,paquete,tamanio_paquete);
    free(paquete);
};

void send_EXITO_OPERACION(int socket){
    payload_EXITO_OPERACION payload;

    int tamanio_paquete;
    char* paquete = pack_EXITO_OPERACION(payload,&tamanio_paquete);
    enviar_paquete(socket,paquete,tamanio_paquete);
    free(paquete);
};

void send_FRACASO_OPERACION(int socket){
    payload_FRACASO_OPERACION payload;

    int tamanio_paquete;
    char* paquete = pack_FRACASO_OPERACION(payload,&tamanio_paquete);
    enviar_paquete(socket,paquete,tamanio_paquete);
    free(paquete);
};

