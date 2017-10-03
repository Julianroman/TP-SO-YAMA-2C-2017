/*
 ============================================================================
 Name        : FileSystem.c
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
#include <pthread.h>
#include "consola.h"

t_log* log;
int miPuerto = 5040;
int cantBloques = 20;
int tamanioBloques = 64;
int bitmap[20];
char* pathBitmap = "Bitmap.bin";

int estadoEstable = 0;
//
bool validarArchivo(char* pathArchivo){
	FILE* archivo;
	archivo = fopen(pathArchivo,"r");
	if (archivo == NULL){
		//printf("No existe el archivo en el FileSystem\n");
			//fclose(archivo);
			return 0;
	}else{
			//printf("SI!!, Existe el archivo en el FileSystem\n");
			fclose(archivo);
			return 1;
	}
}

void crearBitmap() {
	FILE* archivoBitmap;
	archivoBitmap = fopen(pathBitmap,"w");
	int i;

	for (i = 0; i < cantBloques; i++) {
		fwrite("0", 1, 1, archivoBitmap);
		bitmap[i] = 0;
	}
	fclose(archivoBitmap);
}

void inicializarBitmap() {
	if(validarArchivo(pathBitmap) == 1){
		FILE* archivoBitmap;
		archivoBitmap = fopen(pathBitmap,"r");

		fgets(bitmap, sizeof(bitmap), archivoBitmap);
		bitmap[strlen(bitmap)] = '\0';
	}else{
		crearBitmap();
	}

}

int main(void) {
	puts("Comienza el proceso FileSystem");
	log = log_create("fileSystem.log", "FileSystem", true, LOG_LEVEL_TRACE);
	//log_trace(log, "MENSAJE");

	//servidor(miPuerto);

	//init_consola();

	///Creo el hiloConsola que llama a la funcion init_consola()
	pthread_t hiloConsola;
	pthread_create(&hiloConsola, NULL, (void*) init_consola, NULL);

	//Creo el hiloServidor que llama a la funcion servidor(miPuerto)
	pthread_t hiloServidor;
	pthread_create(&hiloServidor, NULL, (void*) servidor, miPuerto);

	//El procSso no termina hasta que mueren los dos hilos
	pthread_join(hiloConsola, NULL);
	pthread_join(hiloServidor, NULL);

	//Para las conexiones, mas adelante falta agregar que si
	//estadoEstable == 0
	//No permita conexiones de Workers o YAMA

	inicializarBitmap();

	return EXIT_SUCCESS;
}
