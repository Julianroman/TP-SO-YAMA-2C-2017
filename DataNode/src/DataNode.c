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
#include <utilidades/protocol/senders.h>

#define TAMANIOBLOQUE 1048576
//
int puertoFs;
int id;
char* ipFs = "";
char* pathDataBin = "";
t_log* logger;
int cantidadDeBloques;

// Prototipos
void leerConfiguracion();
void clienteDatanode(const char* ip, int puerto);
void escribirArchivo(char* data, int size, int nroBloque);
char *leerArchivo(int size, int nroBloque);
void realizarPeticion(void * data, HEADER_T cabecera, int socket);
void crearDataBin();

void leerConfiguracion(){
	char* path = "/home/utnso/workspace/tp-2017-2c-Grupo-1---K3525/DataNode/Debug/nodo-config.cfg";
	//char* path = "nodo-config.cfg";
	t_config* archivo_configuracion = config_create(path);
	puertoFs = config_get_int_value(archivo_configuracion, "PUERTO_FILESYSTEM");
	printf("El puerto FS es: %i \n", puertoFs);
	ipFs = config_get_string_value(archivo_configuracion, "IP_FILESYSTEM");
	printf("La IP FS es: %s \n", ipFs);
	pathDataBin = config_get_string_value(archivo_configuracion, "RUTA_DATABIN");
	printf("La ruta del DataBin es: %s \n", pathDataBin);
	cantidadDeBloques = config_get_int_value(archivo_configuracion, "CANTIDAD_BLOQUES");
	printf("La cantidad de bloques es: %i \n", cantidadDeBloques);
	id = config_get_int_value(archivo_configuracion, "NUMERO_NODO");
	printf("El numero del nodo es: %i \n", id);

}

int main(void) {
	puts("Comienza DataNode");

	///Se crea el log
	logger = log_create("dataNode.log", "DataNode", true, LOG_LEVEL_TRACE);
	log_trace(logger, "Leyendo configuracion");
	leerConfiguracion();
	log_trace(logger, "Configuracion leida");

	/* -------- DEV-FEATURE ---------------------------------------------- */
	/* Opcion de asignar puerto para multiples nodos en el mismo ordenador */

	/*if (argc==2){
		char* idString = argv[1];
		id = atoi(idString);
	}*/
	/* -- END / DEV-FEATURE ---------------------------------------------- */

	crearDataBin();
	/*char* lectura;
	lectura = leerArchivo(TAMANIOBLOQUE, 8);
	//escribirArchivo(lectura, TAMANIOBLOQUE, 3);
	free(lectura);*/
	clienteDatanode(ipFs, puertoFs);
	return EXIT_SUCCESS;
}

void crearDataBin(){
	int archivo;
	if (!(archivo = fopen(pathDataBin, "r"))){
		log_trace(logger, "Archivo inexistente se crea.");
		archivo = fopen(pathDataBin,"w");
		ftruncate(fileno(archivo),TAMANIOBLOQUE*cantidadDeBloques);
		fclose(archivo);
	}
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
			HEADER_T cabecera;
			void* data;
			data = receive(cliente,&cabecera);
			realizarPeticion(data, cabecera, cliente);
	}
}

void realizarPeticion(void * data, HEADER_T cabecera, int socket){
	char* bloque;
	payload_BLOQUE * payloadEscribir;
	payload_PETICION_BLOQUE * payloadLeer;
	switch(cabecera){
	case PETICION_BLOQUE:
		payloadLeer = data;
		log_trace(logger, "lectura del bloque %i, %i bytes", payloadLeer->numero_bloque, payloadLeer->tam_bloque);
		bloque = malloc(payloadLeer->tam_bloque);
		bloque = leerArchivo(payloadLeer->tam_bloque, payloadLeer->numero_bloque);
		printf("Contenido: %s.",bloque);
		send_BLOQUE(socket, payloadLeer->tam_bloque, bloque, payloadLeer->numero_bloque);
		free(bloque);
		break;
	case BLOQUE:
		payloadEscribir = data;
		log_trace(logger, "Escritura en el bloque %i, %i bytes", payloadEscribir->numero_bloque, payloadEscribir->tamanio_bloque);
		printf("Contenido: %s.", payloadEscribir->contenido);
		escribirArchivo(payloadEscribir->contenido, payloadEscribir->tamanio_bloque, payloadEscribir->numero_bloque);
		break;
	case FIN_COMUNICACION:
		printf("Se desconecto el FS.");
		log_trace(logger, "Se desconecto el FS.");
		exit(1);
		break;
	}
}

void escribirArchivo(char* data, int size, int nroBloque){
	int offset = TAMANIOBLOQUE * nroBloque;
	int archivo;
	if (!(archivo = fopen(pathDataBin, "r"))){
		crearDataBin();
	}
	archivo = open(pathDataBin, O_RDWR);
	char * map = mmap((caddr_t)0, size, PROT_WRITE, MAP_SHARED, archivo, offset);
	memcpy(map, data, size);
	close(archivo);
	char* mensajeEscritura = string_from_format("Escritura completa en bloque %d /Tamanio: %d",nroBloque,size);
	log_trace(logger, mensajeEscritura);
	free(mensajeEscritura);
}

char *leerArchivo(int size, int nroBloque){
	int offset = TAMANIOBLOQUE * nroBloque;
	int archivo;
	if (!(archivo = fopen(pathDataBin, "r"))){
		crearDataBin();
	}
	char* lectura = malloc(size);
	archivo = open(pathDataBin, O_RDONLY);
	char * map = mmap((caddr_t)0, size, PROT_READ, MAP_SHARED, archivo, offset);
	//Se guarda en lectura lo leido desde el offset
	memcpy(lectura, map, size);
	lectura[size] = '\0';
	close(archivo);
	printf("El bloque %d dice: %s \n", nroBloque, lectura);
	return lectura;
}
