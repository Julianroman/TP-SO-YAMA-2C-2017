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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <utilidades/socket_utils.h>
#include <utilidades/protocol/senders.h>
#include "etapas/etapas.h"
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
	// Recibir parametros
	if (argc!=2){
		puts("Ingrese la ruta de un archivo");
		return 1;
	}
	char* rutaTransformador = argv[1];

	// Abrir archivo
	int transformador_fd = open(rutaTransformador,NULL);



	// Manejo de logs
	logs = log_create("master.log", "Master", false, LOG_LEVEL_TRACE);
	log_trace(logs, "Comienza proceso master");
	log_trace(logs, "Leyendo configuracion");
	leerConfiguracion();
	log_trace(logs, "Configuracion leida");

	// Conectarse al YAMA
	int socketYAMA = crear_conexion(IPYAMA,puertoYama);
	log_trace(logs, "Conectado al YAMA");

	MASTER_STATUS status;
	// Etapas
		// Transformacion
	status = etapa_transformacion(socketYAMA,rutaTransformador);
	if(status == EXITO){
		puts ("Transformacion exitosa");
		log_trace(logs, "Transformacion exitosa");
	}



	printf("Presione INTRO para terminar...\n");
	getchar();
	return EXIT_SUCCESS;
}
