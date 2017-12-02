/*
 ============================================================================
 Name        : FileSystem.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */


#include "consola.h"
#include "funcionesFS.h"

#include <stdio.h>
#include <string.h>

extern t_log *log;
int miPuerto = 5040; // Puerto de conexion

extern pthread_mutex_t mutexContenido;

int main(int arg, char** argv) {
	log = log_create("fileSystem.log", "FileSystem", true, LOG_LEVEL_TRACE);
	log_trace(log, "Comienza el proceso FileSystem");


	if (arg > 1 && string_equals_ignore_case(argv[1], "--clean")){
		log_info(log,"Se inicia el Filesystem ignorando y eliminando estado anterior");
		formatear();
	}
	else{
		log_info(log,"Se intentara iniciar el Filesystem reestableciendo desde estado anterior");

		initOrRestoreFS();
	}

	pthread_mutex_init(&mutexContenido, NULL);

	pthread_t hiloConsola;
	pthread_create(&hiloConsola, NULL, (void*) init_consola, NULL);

	pthread_t hiloServidor;
	pthread_create(&hiloServidor, NULL, (void*) servidorFs, miPuerto);

	pthread_join(hiloConsola, NULL);
	pthread_join(hiloServidor, NULL);

	//nodosARestaurar();

	//Para las conexiones, mas adelante falta agregar que si
	//No permita conexiones de Workers o YAMA

	/*inicializarNodo(1,5,20);
	inicializarNodo(2,5,40);
	inicializarNodo(3,5,60);*/

	//almacenarArchivo("/home/utnso","root/metadata" ,"archivoBinario","bin");
	//almacenarArchivo("/home/utnso","root/metadata" ,"archivoTexto","txt");
	//leerArchivo("/home/utnso/workspace/tp-2017-2c-Grupo-1---K3525/FileSystem/root/metadata/archivoBinario.bin");

	//cpfrom /home/utnso root/metadata archivoBinario bin
	//cat /home/utnso/workspace/tp-2017-2c-Grupo-1---K3525/FileSystem/root/metadata/archivoBinario.bin
	return EXIT_SUCCESS;
}
