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
//
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

	return EXIT_SUCCESS;
}
