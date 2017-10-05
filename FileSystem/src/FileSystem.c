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
#include <sys/mman.h>
t_log* log;
int miPuerto = 5040;
int cantBloques = 20;
int tamanioBloques = 1048576; //1MB
int bitmap[20];
char* pathBitmap = ""; //Esto deberia estar en la carpeta Metadata

int estadoEstable = 0;
int formateado = 0;
//

void almacenarArchivo(char* pathCompleto, char* nombreArchivo, char* tipo){//Y también recibe "Los datos correspondientes"
	if(strcmp(tipo,"bin") == 0){
		puts("ARCHIVO BINARIO");

	}
	else if(strcmp(tipo,"txt") == 0){
		puts("ARCHIVO DE TEXTO");

	}

}

void formatear(){
	/*if(existeUnEstadoAnterior){
		restaurarDesdeAhi
	}*/


	estadoEstable = 1;
	formateado = 1;
}

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

void crearBitmap(char* pathArchivo) {
	FILE* archivoBitmap;
	archivoBitmap = fopen(pathArchivo,"w");
	int i;

	for (i = 0; i < cantBloques; i++) {
		fwrite("0", 1, 1, archivoBitmap);
		bitmap[i] = 0;
	}
	fclose(archivoBitmap);
}

void inicializarBitmap(char* nombreNodo) {
	char *pathNewBitmap = string_new();
	strcpy(pathNewBitmap, pathBitmap);
	string_append(&pathNewBitmap, nombreNodo);
	string_append(&pathNewBitmap, ".bin");

	if(validarArchivo(pathNewBitmap) == 1){

		FILE* archivoBitmap;
		archivoBitmap = fopen(pathNewBitmap,"r");

		fgets(bitmap, sizeof(bitmap), archivoBitmap);
		bitmap[strlen(bitmap)] = '\0';
	}else{
		crearBitmap(pathNewBitmap);
	}

}
char* mapearArchivo(char *pathArchivo){
	struct stat estado_archivo;
	int fd=open(pathArchivo,R_OK|W_OK);
	char *archivoMapeado;
	if(fd<0){
		perror("open");
		exit(1);
	}
	if(fstat(fd,&estado_archivo)<0){
		perror("fstat");
		exit(1);
	}
	archivoMapeado=mmap(NULL,estado_archivo.st_size,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);
	close(fd);
	return archivoMapeado;
}
int main(void) {
	puts("Comienza el proceso FileSystem");
	log = log_create("fileSystem.log", "FileSystem", true, LOG_LEVEL_TRACE);
	//log_trace(log, "MENSAJE");

	//servidor(miPuerto);

	//init_consola();

	///Creo el hiloConsola que llama a la funcion init_consola()
	/*pthread_t hiloConsola;
	pthread_create(&hiloConsola, NULL, (void*) init_consola, NULL);

	//Creo el hiloServidor que llama a la funcion servidor(miPuerto)
	pthread_t hiloServidor;
	pthread_create(&hiloServidor, NULL, (void*) servidor, miPuerto);

	//El procSso no termina hasta que mueren los dos hilos
	pthread_join(hiloConsola, NULL);
	pthread_join(hiloServidor, NULL);*/

	//Para las conexiones, mas adelante falta agregar que si
	//estadoEstable == 0
	//No permita conexiones de Workers o YAMA




	inicializarBitmap("Nodo1");
	almacenarArchivo("","","bin");

	return EXIT_SUCCESS;
}
