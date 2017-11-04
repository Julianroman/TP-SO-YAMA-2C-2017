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



int main(int arg, char** argv) {
	log = log_create("fileSystem.log", "FileSystem", true, LOG_LEVEL_TRACE);
	log_trace(log, "Comienza el proceso FileSystem");

	listaDeNodos = list_create();
	initTablaDeDirectorios();

	if (argv[1] != NULL && strcmp(argv[1], "--clean")){
		log_info(log,"Iniciar ignorando/eliminando estado anterior");
		//format_fs(configFS,directorios);
	}
	else{
		//log_info(log,"Iniciar reestableciendo desde estado anterior");
		/*restablecerEstado();
		for(int i = 0; i < list_size(archivos); i++){
			t_arch* archivo = list_get(archivos, i);
			printf("%d\n", archivo->padre);
		}*/
	}

	pthread_t hiloConsola;
	pthread_create(&hiloConsola, NULL, (void*) init_consola, NULL);

	pthread_t hiloServidor;
	pthread_create(&hiloServidor, NULL, (void*) servidor, miPuerto);

	pthread_join(hiloConsola, NULL);
	pthread_join(hiloServidor, NULL);

	//Para las conexiones, mas adelante falta agregar que si
	//estadoEstable == 0
	//No permita conexiones de Workers o YAMA

	createDirectory("metadata");
	createDirectory("metadata/bitmaps");

	/*inicializarNodo(2);
	inicializarNodo(1);
	inicializarNodo(10);
	//cantidadTotalBloquesLibres();*/


	//createDirectory("some");
	//createDirectory("some/other");

	//almacenarArchivo("Nodo1.bin","","bin");
	//almacenarArchivo("Nodo10.txt","","txt");
	//importarArchivo("Nodo1.bin","");

	//copiaLocalAlYamafs("/home/utnso/Nuevo.txt", "root");


	return EXIT_SUCCESS;
}
