/*
 * funcionesMaster.c
 *
 *  Created on: 15/11/2017
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <commons/string.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <pthread.h>

#include <utilidades/socket_utils.h>
#include <utilidades/protocol/senders.h>
#include <utilidades/protocol/receive.h>
#include <utilidades/protocol/types.h>

#include "operaciones/operaciones.h"


void leerConfiguracion(char* path, char** ipYama,int* puertoYama){
	t_config* archivo_configuracion = config_create(path);
	*puertoYama = config_get_int_value(archivo_configuracion, "YAMA_PUERTO");
	char* pivote = config_get_string_value(archivo_configuracion, "YAMA_IP");
	*ipYama = malloc(strlen(pivote)+1);
	strcpy(*ipYama, pivote);
	config_destroy(archivo_configuracion);
}

char* scriptToChar(char* path){
	char *file_contents;
	long input_file_size;
	FILE *input_file = fopen(path, "rb");
	fseek(input_file, 0, SEEK_END);
	input_file_size = ftell(input_file);
	rewind(input_file);
	file_contents = malloc(input_file_size * (sizeof(char)));
	fread(file_contents, sizeof(char), input_file_size, input_file);
	fclose(input_file);
	return file_contents;
}

void tituloMaster(){
	printf("\e[1;1H\e[2J");
		puts("\n                   ██╗   ██╗ █████╗ ███╗   ███╗ █████╗ \n                   ╚██╗ ██╔╝██╔══██╗████╗ ████║██╔══██╗\n                    ╚████╔╝ ███████║██╔████╔██║███████║\n                     ╚██╔╝  ██╔══██║██║╚██╔╝██║██╔══██║\n                      ██║   ██║  ██║██║ ╚═╝ ██║██║  ██║\n                      ╚═╝   ╚═╝  ╚═╝╚═╝     ╚═╝╚═╝  ╚═╝\n");
		puts("┬ ┬┌─┐┌┬┐  ┌─┐┌┐┌┌─┐┌┬┐┬ ┬┌─┐┬─┐  ┌┬┐┬─┐  ┌─┐┌┬┐┌┬┐┬┌┐┌┬┌─┐┌┬┐┬─┐┌─┐┌┬┐┌─┐┬─┐\n└┬┘├┤  │   ├─┤││││ │ │ ├─┤├┤ ├┬┘  │││├┬┘  ├─┤ ││││││││││└─┐ │ ├┬┘├─┤ │ │ │├┬┘\n ┴ └─┘ ┴   ┴ ┴┘└┘└─┘ ┴ ┴ ┴└─┘┴└─  ┴ ┴┴└─  ┴ ┴─┴┘┴ ┴┴┘└┘┴└─┘ ┴ ┴└─┴ ┴ ┴ └─┘┴└─");
}

STATUS_MASTER recibirJob(int socketYAMA,int* masterID){
	HEADER_T header;
	payload_JOB* data = receive(socketYAMA,&header);
	if(header != JOB)return DESCONECTADO;
	payload_JOB* job = data;
	*masterID = job ->id_master;
	return EXITO;
}
