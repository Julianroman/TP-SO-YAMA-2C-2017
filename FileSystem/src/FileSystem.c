/*
 ============================================================================
 Name        : FileSystem.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "../../Funciones/Sockets.h"

t_log* log;
int miIP = 5040;

int main(void) {
	puts("Comienza el proceso FileSystem");
	log = log_create("fileSystem.log", "FileSystem", false, LOG_LEVEL_TRACE);
	//log_trace(log, "MENSAJE");

	servidor(miIP);

	return EXIT_SUCCESS;
}
