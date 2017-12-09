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
#define UNMB 1048576;
int TAMANIOBLOQUE = 1048576;

extern t_log* logger;
extern char* nodePath;

char *leerArchivo(int size, int nroBloque, int nodeFD){
	int offset = TAMANIOBLOQUE * nroBloque;
	char* lectura = malloc(size);
	char * map;
	printf("\tDEBUG node:%d size:%d block:%d\n",nodeFD,size,nroBloque);
	if((map = mmap(NULL, size, PROT_READ, MAP_PRIVATE, nodeFD, offset)) == MAP_FAILED){
		perror("mmap");
		log_error(logger,"No se pudo mappear archivo");
	}
	memcpy(lectura, map, size);
	if (munmap(map, size) == -1)
	{
		log_error(logger, "No se pudo liberar el map");
	}
	char* mensajeLectura = string_from_format("Lectura completa en el bloque %i -- %i bytes",nroBloque,size);
	log_trace(logger, mensajeLectura);
	free(mensajeLectura);
	close(nodeFD);
	return lectura;
}

void res_ORDEN_TRANSFORMACION(int socket_cliente,HEADER_T header,void* data){
	pid_t pid = getpid();

	log_info(logger, "Respondiendo ORDEN DE TRANSFORMACION");
	payload_ORDEN_TRANSFORMACION* orden = data;
	HEADER_T cabecera;

	// Recibo el script
	payload_SCRIPT* script = receive(socket_cliente,&cabecera);
	if(cabecera != SCRIPT){
		log_error(logger,"Se esperaba un archivo");
		send_FRACASO_OPERACION(socket_cliente);
		exit(1);
	}

	char* contenido = script -> contenido;

	// Guardo el script
	char* scriptPath = string_from_format("scripts/transformador%d", pid);
    FILE *scriptFile = fopen(scriptPath, "ab");
    if (scriptFile != NULL){
        fputs(contenido, scriptFile);
        fclose(scriptFile);
    }

    // Le otorgo permisos de ejecucion
    char* chmodComand = string_from_format("chmod +x %s", scriptPath);
    system(chmodComand);
    log_info(logger,"Script listo para transformar.");

    char* transformationCommand = string_from_format("dd if=data.bin skip=%d count=%d bs=1 | ./%s | sort > tmp/%s", orden -> bloque, orden -> bytesocupados,scriptPath,orden->nombreArchivoTemporal);
    system(transformationCommand);

	log_trace(logger,"Transformacion OK // bloque: %d / Archivo: %s",(orden ->bloque),(orden->nombreArchivoTemporal));

	// Esito
	send_EXITO_OPERACION(socket_cliente);
	exit(EXIT_SUCCESS);

};

