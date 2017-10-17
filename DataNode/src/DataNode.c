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
//
int puertoFs = 0;
int id = 1;
char* ipFs = "";
t_log* log;

void leerConfiguracion(){
	char* path = "/home/utnso/workspace/tp-2017-2c-Grupo-1---K3525/DataNode/src/nodo-config.cfg";
	t_config* archivo_configuracion = config_create(path);
	puertoFs = config_get_int_value(archivo_configuracion, "PUERTO_FILESYSTEM");
	printf("El puerto FS es: %i \n", puertoFs);
	ipFs = config_get_string_value(archivo_configuracion, "IP_FILESYSTEM");
	printf("La IP FS es: %s \n", ipFs);

	config_destroy(archivo_configuracion);
}

int main(void) {
	puts("Comienza DataNode");
	///Se crea el log
	log = log_create("dataNode.log", "DataNode", false, LOG_LEVEL_TRACE);
	log_trace(log, "Leyendo configuracion");
	leerConfiguracion();
	log_trace(log, "Configuracion leida");

	clienteDatanode(ipFs, puertoFs, id);
	//escribirArchivo("metadata/archivo.txt", "polenta", 3);
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

	char* buffer = malloc(1000);
	while (1) {
		int bytesRecibidos = recv(cliente, buffer, 1000, 0);
		if (bytesRecibidos <= 0) {
			perror("El FS se desconectó");
			exit(1);
		}
		buffer[bytesRecibidos] = '\0';
		printf("%s dice: %s\n", tipo_proceso(0), buffer);
	}
	free(buffer);
}

void escribirArchivo(char* path, char* data, int offset){
	FILE* archivo;
	if (!(archivo = fopen(path, "r"))){
		log_error(log, "El archivo no existe o no se pudo abrir");
	}else{
		archivo = fopen(path,"rb+");
		fseek(archivo, offset, SEEK_SET);
		fwrite(data, strlen(data), 1, archivo);
		fclose(archivo);
		puts("Escritura Completa");
	}

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
