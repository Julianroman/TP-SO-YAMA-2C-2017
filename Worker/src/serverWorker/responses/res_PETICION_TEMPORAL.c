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

char *leerTemporal(int size, char* path){
	int offset = 0;
	int archivo;
	char* lectura = malloc(size);
	archivo = open(path, O_RDONLY);
	char * map;
	if((map = mmap((caddr_t)0, size, PROT_READ, MAP_SHARED, archivo, offset)) == MAP_FAILED){
		log_error(logger,"No se pudo mappear archivo");
	}
	memcpy(lectura, map, size);
	if (munmap(map, size) == -1)
	{
		log_error(logger, "No se pudo liberar el map");
	}
	close(archivo);
	return lectura;
}

void res_PETICION_TEMPORAL(int socket_cliente,HEADER_T header,void* data){
	log_info(logger, "Respondiendo PETICION ARCHIVO TEMPORAL");
	payload_PETICION_TEMPORAL* orden = data;

	// Cargar el temporal local
	char* temporalPath = string_from_format("tmp/%s",orden -> nombre);

	int input_file_size;
	FILE *input_file = fopen(temporalPath, "rb");

	fseek(input_file, 0, SEEK_END);
	input_file_size = ftell(input_file);
	fclose(input_file);

	char * temporal = leerTemporal(input_file_size, temporalPath);

	free(temporalPath);
	int i;
	for(i = 0; i < input_file_size; i++){
		putchar(temporal[i]);
	}

	send_BLOQUE(socket_cliente,input_file_size,temporal,-1);
	log_trace(logger, "Archivo enviado");

	exit(0);


};

