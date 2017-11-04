/*
 ============================================================================
 Name        : DataNode.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <utilidades/Sockets.c>
#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>
#include <utilidades/protocol/receive.h>

#define TAMANIOBLOQUE 1024
//
int puertoFs = 0;
int id = 1;
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

	config_destroy(archivo_configuracion);
}

int main(void) {
	puts("Comienza DataNode");
	///Se crea el log
	log = log_create("dataNode.log", "DataNode", false, LOG_LEVEL_TRACE);
	log_trace(log, "Leyendo configuracion");
	leerConfiguracion();
	log_trace(log, "Configuracion leida");

	//clienteDatanode(ipFs, puertoFs, id);
	escribirArchivo("metadata/bloquesDataNode.txt", "polenta", 7, 7);
	//leerArchivo("metadata/archivo.txt", 7, 7);
	return EXIT_SUCCESS;
}


void clienteDatanode(const char* ip, int puerto, int id_tipo_proceso){
	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = INADDR_ANY;
	//direccionServidor.sin_addr.s_addr = inet_addr(ip);
	direccionServidor.sin_port = htons(puerto);

	int cliente = socket(AF_INET, SOCK_STREAM, 0);
	if (connect(cliente, (void*) &direccionServidor, sizeof(direccionServidor)) != 0) {
		perror("No se pudo conectar");
		exit(1);
	}

	int numeroConvertido = htonl(id_tipo_proceso);
	send(cliente, &numeroConvertido, sizeof(numeroConvertido), 0);

	void* payload;
	int id_bloque;
	while (1) {
		HEADER_T* header;
		payload = receive(cliente, header);
		//este id deberia estan en el header
		//id_bloque = header->id o como sea;
		escribirArchivo("metadata/bloquesDataNode.txt", payload, id_bloque);
		printf("%s dice: %s\n", tipo_proceso(0), payload);
	}
}

void escribirArchivo(char* path, char* data, int size, int nroBloque){
	int offset;
	//offset = TAMANIOBLOQUE * nroBloque;
	offset = 7;
	FILE* archivo;
	if (!(archivo = fopen(path, "r"))){
		log_error(log, "El archivo inexistente se crea");
		archivo = fopen(path,"w");
		truncate(fileno(archivo),cantidadDeBloques*TAMANIOBLOQUE);
		fclose(archivo);
	}

	archivo = fopen(path,"rb+");
	//int sizeFile = ftell(archivo);
	fseek(archivo, offset, SEEK_SET);
	fwrite(data, size, 1, archivo);
	fflush(archivo);

	fclose(archivo);
	puts("Escritura Completa");
}

void leerArchivo(char* path, int inicial, int offset){
	FILE* archivo;
	char* lectura;
	if (!(archivo = fopen(path, "r"))){
		log_error(log, "El archivo no existe o no se pudo abrir");
	}else{
		archivo = fopen(path,"r");
		fseek(archivo, inicial, SEEK_SET);
		//Se lee desde inicial hasta el desplazamiento
		fread(lectura, offset, 1, archivo);
		//Se guarda en lectura lo leido
		fclose(archivo);
		printf("%s", lectura);
	}

}

