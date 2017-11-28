/*
 ============================================================================
 /*
 ============================================================================
 Name        : DataNode.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include <commons/config.h>
#include <commons/string.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <utilidades/protocol/receive.h>

#define TAMANIOBLOQUE 1048576
#define PATHPOSTA "DataNode.dat"
//
int puertoFs;
int id;
char* ipFs = "";
t_log* log;
int cantidadDeBloques;

void leerConfiguracion(){
	char* path = "/home/utnso/workspace/tp-2017-2c-Grupo-1---K3525/DataNode/src/nodo-config.cfg";
	t_config* archivo_configuracion = config_create(path);
	puertoFs = config_get_int_value(archivo_configuracion, "PUERTO_FILESYSTEM");
	printf("El puerto FS es: %i \n", puertoFs);
	ipFs = config_get_string_value(archivo_configuracion, "IP_FILESYSTEM");
	printf("La IP FS es: %s \n", ipFs);
	cantidadDeBloques = config_get_int_value(archivo_configuracion, "CANTIDAD_BLOQUES");
	printf("La cantidad de bloques es: %i \n", cantidadDeBloques);
	id = config_get_int_value(archivo_configuracion, "NUMERO_NODO");
	printf("El numero del nodo es: %i \n", id);

}

int main(void) {
	puts("Comienza DataNode");
	///Se crea el log
	log = log_create("dataNode.log", "DataNode", false, LOG_LEVEL_TRACE);
	log_trace(log, "Leyendo configuracion");
	leerConfiguracion();
	log_trace(log, "Configuracion leida");

	clienteDatanode(ipFs, puertoFs);
	//char * data = "juli puto";
	//int size = sizeof(char*) * strlen(data);
	//escribirArchivo(PATHPOSTA, data, strlen(data), 3);
	//leerArchivo(PATHPOSTA, strlen(data), 3);
	return EXIT_SUCCESS;
}


void clienteDatanode(const char* ip, int puerto){
	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = inet_addr(ip);
	direccionServidor.sin_port = htons(puerto);

	int cliente = socket(AF_INET, SOCK_STREAM, 0);
	if (connect(cliente, (void*) &direccionServidor, sizeof(direccionServidor)) != 0) {
		perror("No se pudo conectar");
		exit(1);
	}

	//------- Mensaje de bienvenida del FileSystem ---------------
	char* buffer = malloc(1000);
	int bytesRecibidos = recv(cliente, buffer, 1000, 0);
	buffer[bytesRecibidos] = '\0';
	printf("FileSystem dice: %s\n", buffer);
	free(buffer);
	//------------------------------------------------------------

	send_PRESENTACION_DATANODE(cliente, 1, id, cantidadDeBloques);
	//TODO: aca se queda escuchando para recibir bloques
	while (1) {
		HEADER_T* cabecera;
		void* data;
		data = receive(cliente,&cabecera);
		payload_BLOQUE * payload = data;
		escribirArchivo(PATHPOSTA, payload->contenido, payload->tamanio_bloque, 1);
		//printf("Datanode %d dice: %s\n", payload->id_bloque, payload->bloque);
	}
}

void escribirArchivo(char* path, char* data, int size, int nroBloque){
	int offset = TAMANIOBLOQUE * nroBloque;
	int archivo;
	if (!(archivo = fopen(path, "r"))){
		log_trace(log, "Archivo inexistente se crea.");
		archivo = fopen(path,"w");
		ftruncate(fileno(archivo),TAMANIOBLOQUE*cantidadDeBloques);
		fclose(archivo);
	}
	archivo = open(path, O_RDWR);
	char * map = mmap((caddr_t)0, size, PROT_WRITE, MAP_SHARED, archivo, offset);
	memcpy(map, data, size);
	close(archivo);
	puts("Escritura Completa");
}

void leerArchivo(char* path, int size, int nroBloque){
	int offset = TAMANIOBLOQUE * nroBloque;
	int archivo;
	if (!(archivo = fopen(path, "r"))){
		log_error(log, "Archivo inexistente.");
	}
	char* lectura = malloc(size);
	archivo = open(path, O_RDONLY);
	char * map = mmap((caddr_t)0, size, PROT_READ, MAP_SHARED, archivo, offset);
	//Se guarda en lectura lo leido desde el offset
	memcpy(lectura, map, size);
	lectura[size] = '\0';
	close(archivo);
	printf("El bloque %d dice: %s \n", nroBloque, lectura);
	free(lectura);
}

