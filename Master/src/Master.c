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
#include <commons/string.h>
#include <string.h>
#include <unistd.h>
#include <utilidades/socket_utils.h>
#include <utilidades/protocol/senders.h>

#define NOMBREARCH "foo/bar.rb" // TODO recibir por consola
// Eliminar define en cuanto la funcion log devuelva la ip
#define IPYAMA "127.0.0.1"

int puertoYama = 0;
char* ipYama = "";
t_log* logs;

void leerConfiguracion(){
	char* path = "/home/utnso/workspace/tp-2017-2c-Grupo-1---K3525/Master/src/master-config.cfg";
	t_config* archivo_configuracion = config_create(path);
	puertoYama = config_get_int_value(archivo_configuracion, "YAMA_PUERTO");
	printf("El puerto YAMA es: %i \n", puertoYama);
	ipYama = config_get_string_value(archivo_configuracion, "YAMA_IP");
	printf("La IP YAMA es: %s \n", ipYama);
	config_destroy(archivo_configuracion);
}


int main(int argc, char **argv) {
	if (argc!=2){
		puts("Ingrese la ruta de un archivo");
		return 1;
	}
	char* rutaArchivo = argv[1];

	puts("Comienza el proceso Master \n");

	// Manejo de logs
	logs = log_create("master.log", "Master", false, LOG_LEVEL_TRACE);
	log_trace(logs, "Leyendo configuracion");
	leerConfiguracion();
	log_trace(logs, "Configuracion leida");

	// Conectarse al YAMA
	int socketYAMA = crear_conexion(IPYAMA,puertoYama);
	printf("YAMA socket en : %d \n",socketYAMA);

	//Enviar Solicitud
	send_SOLICITUDPROCESAMIENTO(socketYAMA,rutaArchivo);


	printf("Presione INTRO para terminar...\n");
	getchar();
	return EXIT_SUCCESS;
}
