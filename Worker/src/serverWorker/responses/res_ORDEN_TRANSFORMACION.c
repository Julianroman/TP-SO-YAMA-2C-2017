/*
 * ============================================================================
 * res_orden_transformacion.c
 * ============================================================================
 */

#include <utilidades/protocol/senders.h>
#include <utilidades/protocol/receive.h>
#include <utilidades/protocol/destroy.h>
#include <utilidades/protocol/types.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/log.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>

#define BUFFERSIZE 1024
#define UNMB 1048576;
int TAMANIOBLOQUE = 1048576;

extern t_log* logger;

char *leerArchivo(int size, int nroBloque, int nodeFD){
	int offset = TAMANIOBLOQUE * nroBloque;
	char* lectura = malloc(size);
	char * map;
	if((map = mmap(NULL, size, PROT_READ, MAP_PRIVATE, nodeFD, offset)) == MAP_FAILED){
		perror("mmap");
		return NULL;
	}
	memcpy(lectura, map, size);
	if (munmap(map, size) == -1)
	{
		log_error(logger, "No se pudo liberar el map");
		return NULL;
	}
	close(nodeFD);
	return lectura;
}

void res_ORDEN_TRANSFORMACION(int socket_cliente,HEADER_T header,void* data){
	pid_t pid = getpid();

	log_info(logger, "Respondiendo ORDEN DE TRANSFORMACION");
	payload_ORDEN_TRANSFORMACION* orden = data;
	HEADER_T cabecera;

	// Recibo el script
	payload_BLOQUE* script = receive(socket_cliente,&cabecera);
	if(cabecera != BLOQUE){
		log_error(logger,"No se pudo recibir el script");
		send_FRACASO_OPERACION(socket_cliente);
		exit(1);
	}

	char* contenido = script -> contenido;

	// Guardo el script
	char* scriptPath = string_from_format("scripts/transformador%d", pid);
    FILE *scriptFile = fopen(scriptPath, "w+");
    if (scriptFile == NULL){
    	log_error(logger,"Transformacion ERR | No se puede guardar el script | Bloque: %d",(orden -> bloque));
    	exit(1);
    }
	fwrite(contenido,script -> tamanio_bloque,1,scriptFile);
	fclose(scriptFile);
	destroy_BLOQUE(script);

    // Le otorgo permisos de ejecucion
    char* chmodComand = string_from_format("chmod +x %s", scriptPath);
    system(chmodComand);
    free(chmodComand);

	t_config* archivo_configuracion = config_create("worker-config.cfg");
	char* dataBinPath = config_get_string_value(archivo_configuracion, "RUTA_DATABIN");

    // Obtengo el bloque a transformar
    int dataBin = open(dataBinPath, O_RDONLY);
    char * bloqueATransformar = leerArchivo(orden->bytesocupados, orden->bloque, dataBin);
    if(bloqueATransformar == NULL){
    	log_error(logger,"Transformacion ERR | No se pudo recuperar el bloque | Bloque: %d",(orden -> bloque));
    	exit(1);
    }
    close(dataBin);
	config_destroy(archivo_configuracion);

    // Creo un archivo intermedio
    char* temporalPath = string_from_format("tmp/temporalTransformacion%d",pid);
    FILE * temporalFile = fopen(temporalPath,"w+");
    if ((fwrite(bloqueATransformar,1,orden->bytesocupados, temporalFile))<(orden->bytesocupados)){
    	log_error(logger,"Transformacion ERR | No se pudo crear archivo intermedio | Bloque: %d",(orden -> bloque));
    	exit(1);
    }
    free(bloqueATransformar);
    fclose(temporalFile);

    // Ejecuto la transformacion
    log_info(logger,"Transformando bloque %d, %d bytes..",(orden -> bloque),orden -> bytesocupados);
    char* transformationCommand = string_from_format("cat %s | ./%s | sort > tmp/%s",temporalPath, scriptPath ,orden->nombreArchivoTemporal);
    system(transformationCommand);

    // Log intenso
	log_trace(logger,"Transformacion OK | Bloque: %d / Archivo: %s",(orden -> bloque),(orden->nombreArchivoTemporal));

	// Esito
    free(temporalPath);
	send_EXITO_OPERACION(socket_cliente);
	exit(EXIT_SUCCESS);

};
