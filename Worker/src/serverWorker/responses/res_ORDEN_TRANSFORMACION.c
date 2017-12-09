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


    // Cargar archivo a procesar

    //off_t offset = (orden->bloque) * UNMB;// Seleccion de bloque

    // Abrir archivo y conocer sus propiedades
    int nodeFD = open(nodePath,O_RDWR);
    if(nodeFD == -1){
    	log_error(logger,"No se pudo abrir el data.bin");
    	exit(1);
    }
    struct stat nodeStats;
    fstat(nodeFD, &nodeStats);
    //size_t nodeLenght = nodeStats.st_size;

    // Cargarlo en memoria
    //void * node = mmap(NULL,nodeLenght, PROT_READ | PROT_WRITE, MAP_SHARED,nodeFD,0);
    char * node = leerArchivo(orden -> bytesocupados, orden->bloque,nodeFD);
    if(node == NULL){
    	exit(1);
    }

    //PIPEO INTENSIFIES!!!
    int pipe_padreAHijo[2];
    int pipe_hijoAPadre[2];

    pipe(pipe_padreAHijo);
    pipe(pipe_hijoAPadre);

    int status;
    // Un buffer para leer

    if ((pid=fork()) == 0 ){
    	log_info(logger,"Transformando...");
    	//Hijo
    	// Copio las pipes que necesito a stdin y stdout
      	dup2(pipe_padreAHijo[0],STDIN_FILENO);
      	dup2(pipe_hijoAPadre[1],STDOUT_FILENO);

      	// Ciello lo que no necesito
       	close( pipe_padreAHijo[1]);
      	close( pipe_hijoAPadre[0]);
    	close( pipe_hijoAPadre[1]);
    	close( pipe_padreAHijo[0]);

    	// Ejecuto el script
        char *argv[] = {NULL};
        char *envp[] = {NULL};

        execve(scriptPath, argv, envp);
        free(scriptPath);
        exit(1);
    }else{
    	//size_t lenght = (orden -> bytesocupados);
    	//Padre
    	// Cierro lo que no necesito
    	close( pipe_padreAHijo[0] );
    	close( pipe_hijoAPadre[1] );

    	// Escribo
		write(pipe_padreAHijo[1],node,orden -> bytesocupados);

    	// Cierro pipe
    	close( pipe_padreAHijo[1]);

    	// Espero al hijo
    	waitpid(pid,&status,0);

    	// Creo un archivo
    	log_info(logger,"Generando archivo...");
    	char* temporalPath = string_from_format("tmp/%s",orden->nombreArchivoTemporal);
		FILE* fd = fopen(temporalPath,"w+");

    	// Leo de a un char
    	char bufferTemp;
		// Leo de la pipe y escribo en el archivo
    	while(0 != read( pipe_hijoAPadre[0], &bufferTemp, 1)){
			fputc(bufferTemp,fd);
    	}

    	// Cierro todito
    	close(pipe_hijoAPadre[0]);
		fclose(fd);
	    //remove(scriptPath);

	    // Ordeno el archivo
	    char* sortCommand = string_from_format("sort %s -o %s",temporalPath,temporalPath);
	    system(sortCommand);
	    free(sortCommand);
		free(temporalPath);

		log_trace(logger,"Transformacion OK // bloque: %d / Archivo: %s",(orden ->bloque),(orden->nombreArchivoTemporal));

	    // Esito
		send_EXITO_OPERACION(socket_cliente);
		exit(EXIT_SUCCESS);
    }

};

