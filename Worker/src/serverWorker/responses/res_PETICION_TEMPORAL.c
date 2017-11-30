/*
 * ============================================================================
 * res_orden_transformacion.c
 * ============================================================================
 */

#include <stdio.h>
#include <utilidades/protocol/senders.h>
#include <utilidades/protocol/types.h>
#include <utilidades/protocol/receive.h>
#include <commons/log.h>
#include <commons/string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/wait.h>

#define BUFFERSIZE 1024


extern t_log* logger;

char* tempToChar(char* path){
	char *file_contents;
	long input_file_size;
	FILE *input_file = fopen(path, "rb");
	if(input_file == NULL){
		puts("No se pudo leer el archivo");
		exit(1);
	}
	fseek(input_file, 0, SEEK_END);
	input_file_size = ftell(input_file);
	rewind(input_file);
	file_contents = malloc(input_file_size * (sizeof(char)));
	fread(file_contents, sizeof(char), input_file_size, input_file);
	fclose(input_file);
	return file_contents;
}

void res_PETICION_TEMPORAL(int socket_cliente,HEADER_T header,void* data){
	log_info(logger, "Respondiendo PETICION_TEMPORAL");
	payload_PETICION_TEMPORAL* orden = data;

	// Cargar el temporal local
	char* temporalPath = string_from_format("tmp/%s",orden -> nombre);
	char* temporalContents = tempToChar(temporalPath);
	free(temporalPath);

	send_TEMPORAL(socket_cliente,temporalContents);

};

