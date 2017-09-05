/*
 ============================================================================
 Name        : Master.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <parser/metadata_program.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/config.h>

#include <string.h>

int puertoYama;
char* ipYama;

void leerConfiguracion(){
	char* path = "master-config.cfg";
	t_config * archivo_configuracion = config_create(path);
	puertoYama = config_get_int_value(archivo_configuracion, "YAMA_PUERTO");
	ipYama = config_get_string_value(archivo_configuracion, "YAMA_IP");
}

int main(void) {
	puts("Comienza el proceso Master");

	// Obtiene el archivo de configuracion
	leerConfiguracion();

	puts(ipYama);

	return EXIT_SUCCESS;
}
