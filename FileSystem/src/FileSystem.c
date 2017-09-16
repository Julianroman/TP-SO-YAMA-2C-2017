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
#include "consola.h"

t_log* log;
int miIP = 5040;

int main(void) {
	puts("Comienza el proceso FileSystem");
	log = log_create("fileSystem.log", "FileSystem", false, LOG_LEVEL_TRACE);
	//log_trace(log, "MENSAJE");

	servidor(miIP);

	//init_consola();

	return EXIT_SUCCESS;
}
