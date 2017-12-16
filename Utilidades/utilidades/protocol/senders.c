
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

void send_ORDEN_TRANSFORMACION(int socket , int bloque , int bytesocupados , char* nombreArchivoTemporal){
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

void send_ORDEN_REDUCCIONGLOBAL(int socket , int PUERTO_Nodo , char* IP_Nodo , char* nombreTemporal_ReduccionLocal , char* nombreTemporal_ReduccionGlobal , int encargado){
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

void send_ORDEN_ALMACENAMIENTO(int socket , char* rutaAlmacenamiento , char* nombreAlamcenamiento , char* nombreTemporal_ReduccionGlobal){
    payload_ORDEN_ALMACENAMIENTO payload;
    payload.tamanio_rutaAlmacenamiento = (strlen(rutaAlmacenamiento)+1)*sizeof(char);
    payload.rutaAlmacenamiento = rutaAlmacenamiento; 
    payload.tamanio_nombreAlamcenamiento = (strlen(nombreAlamcenamiento)+1)*sizeof(char);
    payload.nombreAlamcenamiento = nombreAlamcenamiento; 
    payload.tamanio_nombreTemporal_ReduccionGlobal = (strlen(nombreTemporal_ReduccionGlobal)+1)*sizeof(char);
    payload.nombreTemporal_ReduccionGlobal = nombreTemporal_ReduccionGlobal; 

    int tamanio_paquete;
    char* paquete = pack_ORDEN_ALMACENAMIENTO(payload,&tamanio_paquete);
    enviar_paquete(socket,paquete,tamanio_paquete);
    free(paquete);
};

void send_INFO_TRANSFORMACION(int socket , int PUERTO_Worker , char* IP_Worker , int bloque , int bytesocupados , char* nombreArchivoTemporal , int ID_Nodo){
    payload_INFO_TRANSFORMACION payload;
    payload.PUERTO_Worker = PUERTO_Worker; 
    payload.tamanio_IP_Worker = (strlen(IP_Worker)+1)*sizeof(char);
    payload.IP_Worker = IP_Worker; 
    payload.bloque = bloque; 
    payload.bytesocupados = bytesocupados; 
    payload.tamanio_nombreArchivoTemporal = (strlen(nombreArchivoTemporal)+1)*sizeof(char);
    payload.nombreArchivoTemporal = nombreArchivoTemporal; 
    payload.ID_Nodo = ID_Nodo; 

    int tamanio_paquete;
    char* paquete = pack_INFO_TRANSFORMACION(payload,&tamanio_paquete);
    enviar_paquete(socket,paquete,tamanio_paquete);
    free(paquete);
};

void send_INFO_REDUCCIONLOCAL(int socket , int PUERTO_Worker , char* IP_Worker , char* nombreTemporal_Transformacion , char* nombreTemporal_ReduccionLocal , int ID_Nodo){
    payload_INFO_REDUCCIONLOCAL payload;
    payload.PUERTO_Worker = PUERTO_Worker; 
    payload.tamanio_IP_Worker = (strlen(IP_Worker)+1)*sizeof(char);
    payload.IP_Worker = IP_Worker; 
    payload.tamanio_nombreTemporal_Transformacion = (strlen(nombreTemporal_Transformacion)+1)*sizeof(char);
    payload.nombreTemporal_Transformacion = nombreTemporal_Transformacion; 
    payload.tamanio_nombreTemporal_ReduccionLocal = (strlen(nombreTemporal_ReduccionLocal)+1)*sizeof(char);
    payload.nombreTemporal_ReduccionLocal = nombreTemporal_ReduccionLocal; 
    payload.ID_Nodo = ID_Nodo; 

    int tamanio_paquete;
    char* paquete = pack_INFO_REDUCCIONLOCAL(payload,&tamanio_paquete);
    enviar_paquete(socket,paquete,tamanio_paquete);
    free(paquete);
};

void send_INFO_REDUCCIONGLOBAL(int socket , int PUERTO_Worker , char* IP_Worker , char* nombreTemporal_ReduccionLocal , char* nombreTemporal_ReduccionGlobal , int encargado , int ID_Nodo){
    payload_INFO_REDUCCIONGLOBAL payload;
    payload.PUERTO_Worker = PUERTO_Worker; 
    payload.tamanio_IP_Worker = (strlen(IP_Worker)+1)*sizeof(char);
    payload.IP_Worker = IP_Worker; 
    payload.tamanio_nombreTemporal_ReduccionLocal = (strlen(nombreTemporal_ReduccionLocal)+1)*sizeof(char);
    payload.nombreTemporal_ReduccionLocal = nombreTemporal_ReduccionLocal; 
    payload.tamanio_nombreTemporal_ReduccionGlobal = (strlen(nombreTemporal_ReduccionGlobal)+1)*sizeof(char);
    payload.nombreTemporal_ReduccionGlobal = nombreTemporal_ReduccionGlobal; 
    payload.encargado = encargado; 
    payload.ID_Nodo = ID_Nodo; 

    int tamanio_paquete;
    char* paquete = pack_INFO_REDUCCIONGLOBAL(payload,&tamanio_paquete);
    enviar_paquete(socket,paquete,tamanio_paquete);
    free(paquete);
};

void send_INFO_ALMACENAMIENTO(int socket , int PUERTO_Worker , char* IP_Worker , char* nombreTemporal_ReduccionGlobal , int ID_Nodo){
    payload_INFO_ALMACENAMIENTO payload;
    payload.PUERTO_Worker = PUERTO_Worker; 
    payload.tamanio_IP_Worker = (strlen(IP_Worker)+1)*sizeof(char);
    payload.IP_Worker = IP_Worker; 
    payload.tamanio_nombreTemporal_ReduccionGlobal = (strlen(nombreTemporal_ReduccionGlobal)+1)*sizeof(char);
    payload.nombreTemporal_ReduccionGlobal = nombreTemporal_ReduccionGlobal; 
    payload.ID_Nodo = ID_Nodo; 

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

void send_NODO(int socket , int PUERTO_Nodo , char* IP_Nodo , char* nombreNodo){
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
	//
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
void send_BLOQUE(int socket, int tamanio_bloque, char* bloque, int id_bloque){
	payload_BLOQUE payload;
	payload.tamanio_bloque = tamanio_bloque;
	payload.contenido = bloque;
	payload.numero_bloque = id_bloque;

	int tamanio_paquete;
	char* paquete = pack_BLOQUE(payload,&tamanio_paquete);
	enviar_paquete(socket,paquete,tamanio_paquete);
	free(paquete);
};
void send_PRESENTACION_DATANODE(int socket , int id_dataNode , int cantidad_bloques , char* ipDatanode){
    payload_PRESENTACION_DATANODE payload;
    payload.id_dataNode = id_dataNode; 
    payload.cantidad_bloques = cantidad_bloques; 
    payload.tamanio_ipDatanode = (strlen(ipDatanode)+1)*sizeof(char);
    payload.ipDatanode = ipDatanode; 

    int tamanio_paquete;
    char* paquete = pack_PRESENTACION_DATANODE(payload,&tamanio_paquete);
    enviar_paquete(socket,paquete,tamanio_paquete);
    free(paquete);
};

void send_JOB(int socket , int id_master){
    payload_JOB payload;
    payload.id_master = id_master; 

    int tamanio_paquete;
    char* paquete = pack_JOB(payload,&tamanio_paquete);
    enviar_paquete(socket,paquete,tamanio_paquete);
    free(paquete);
};

void send_RESPUESTA_MASTER(int socket , int id_master , int id_nodo , int bloque , int estado){
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

void send_SCRIPT(int socket , char* contenido){
    payload_SCRIPT payload;
    payload.tamanio_contenido = (strlen(contenido)+1)*sizeof(char);
    payload.contenido = contenido; 

    int tamanio_paquete;
    char* paquete = pack_SCRIPT(payload,&tamanio_paquete);
    enviar_paquete(socket,paquete,tamanio_paquete);
    free(paquete);
};

void send_PETICION_BLOQUE(int socket , int numero_bloque , int tam_bloque){
    payload_PETICION_BLOQUE payload;
    payload.numero_bloque = numero_bloque; 
    payload.tam_bloque = tam_bloque; 

    int tamanio_paquete;
    char* paquete = pack_PETICION_BLOQUE(payload,&tamanio_paquete);
    enviar_paquete(socket,paquete,tamanio_paquete);
    free(paquete);
};

void send_UBICACION_BLOQUE(int socket , char* ip , int puerto , int numero_nodo , int bloque_nodo , int bloque_archivo , int copia , int tam_bloque){
    payload_UBICACION_BLOQUE payload;
    payload.tamanio_ip = (strlen(ip)+1)*sizeof(char);
    payload.ip = ip; 
    payload.puerto = puerto; 
    payload.numero_nodo = numero_nodo; 
    payload.bloque_nodo = bloque_nodo; 
    payload.bloque_archivo = bloque_archivo; 
    payload.copia = copia; 
    payload.tam_bloque = tam_bloque; 

    int tamanio_paquete;
    char* paquete = pack_UBICACION_BLOQUE(payload,&tamanio_paquete);
    enviar_paquete(socket,paquete,tamanio_paquete);
    free(paquete);
};

void send_TEMPORAL(int socket , char* contenido){
    payload_TEMPORAL payload;
    payload.tamanio_contenido = (strlen(contenido)+1)*sizeof(char);
    payload.contenido = contenido; 

    int tamanio_paquete;
    char* paquete = pack_TEMPORAL(payload,&tamanio_paquete);
    enviar_paquete(socket,paquete,tamanio_paquete);
    free(paquete);
};

void send_PETICION_TEMPORAL(int socket , char* nombre){
    payload_PETICION_TEMPORAL payload;
    payload.tamanio_nombre = (strlen(nombre)+1)*sizeof(char);
    payload.nombre = nombre; 

    int tamanio_paquete;
    char* paquete = pack_PETICION_TEMPORAL(payload,&tamanio_paquete);
    enviar_paquete(socket,paquete,tamanio_paquete);
    free(paquete);
};

void send_RECHAZO_CONEXION(int socket){
    payload_RECHAZO_CONEXION payload;

    int tamanio_paquete;
    char* paquete = pack_RECHAZO_CONEXION(payload,&tamanio_paquete);
    enviar_paquete(socket,paquete,tamanio_paquete);
    free(paquete);
};

void send_ALMACENAR_ARCHIVO(int socket, int tamanio_contenido, char* contenido, char* pathDestino,char* nombre,char* tipo){
	    payload_ALMACENAR_ARCHIVO payload;
	    payload.tamanio_contenido = tamanio_contenido;
	    payload.contenido = contenido; 
	    payload.tamanio_pathDestino = (strlen(pathDestino)+1)*sizeof(char);
	    payload.pathDestino = pathDestino; 
	    payload.tamanio_nombre = (strlen(nombre)+1)*sizeof(char);
	    payload.nombre = nombre; 
	    payload.tamanio_tipo = (strlen(tipo)+1)*sizeof(char);
	    payload.tipo = tipo; 
	

	int tamanio_paquete;
	char* paquete = pack_ALMACENAR_ARCHIVO(payload,&tamanio_paquete);
	enviar_paquete(socket,paquete,tamanio_paquete);
	free(paquete);
};
