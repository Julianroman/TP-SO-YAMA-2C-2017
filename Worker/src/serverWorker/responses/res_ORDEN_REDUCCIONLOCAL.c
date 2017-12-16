/*
 * ============================================================================
 * res_orden_reduxLocal.c
 * ============================================================================
 */

#include <stdio.h>
#include <utilidades/protocol/senders.h>
#include <utilidades/protocol/types.h>
#include <utilidades/protocol/receive.h>
#include <utilidades/protocol/destroy.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/collections/list.h>
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

typedef struct{
	FILE* file;
	char* lastLine;
} t_transformado;

void t_transformado_destroyer(void* victima){
	t_transformado* transformado = victima;

	fclose(transformado -> file);
	free(transformado -> lastLine);
}

void res_ORDEN_REDUCCIONLOCAL(int socket_cliente,HEADER_T header,void* data){

	log_info(logger, "Respondiendo ORDEN DE REDUCCION LOCAL");

	pid_t pid = getpid();
    char* transformadoPath;
	payload_ORDEN_REDUCCIONLOCAL* orden = data;
	HEADER_T cabecera;


	// Guardar el nombre final
	char* nombreReduccionLocal = string_from_format("%s",orden -> nombreTemporal_ReduccionLocal);


	// Generar el path
	transformadoPath = string_from_format("tmp/%s",orden->nombreTemporal_Transformacion);
	char* transformados = string_new();
	string_append(&transformados,transformadoPath);


    // Destruir orden cuando no la uso
	destroy_ORDEN_REDUCCIONLOCAL(orden);

	// Recibir las distintas reducciones en el mismo nodo
	orden = receive(socket_cliente,&header);
	while(header == ORDEN_REDUCCIONLOCAL){

		// Generar el path
		transformadoPath = string_from_format("tmp/%s",orden->nombreTemporal_Transformacion);
		string_append(&transformados," ");
		string_append(&transformados,transformadoPath);

	    // Destruir orden cuando no la uso
		destroy_ORDEN_REDUCCIONLOCAL(orden);

		// Recibir proxima orden
		orden = receive(socket_cliente,&header);
	}
	// Aseguro que termino la lista
	if(header != FIN_LISTA){
		send_FRACASO_OPERACION(socket_cliente);
		exit(1);
	}
	// Destruyo el fin de list


	// Recibir script
	payload_BLOQUE* script = receive(socket_cliente,&cabecera);
	if(cabecera != BLOQUE){
		log_error(logger,"Se esperaba un archivo");
		send_FRACASO_OPERACION(socket_cliente);
		exit(1);
	}


	log_info(logger,"Reductor recibido");

	// Guardo el script
	char* contenido = script -> contenido;
	char* scriptPath = string_from_format("scripts/reductor%d", pid);
    FILE *fp = fopen(scriptPath, "w+");
    if (fp != NULL){
        fwrite(contenido,script ->tamanio_bloque,1,fp);
        fclose(fp);
    }

    // Le otorgo permisos de ejecucion
    // la funcion chmod no me estaria funcionando pero el buen system siempre provee
    char* chmodComand = string_from_format("chmod 777 %s", scriptPath);
    system(chmodComand);

    char* comandoReduccion = string_from_format("sort -m %s | ./%s > tmp/%s",transformados,scriptPath,nombreReduccionLocal);
    system(comandoReduccion);

    log_trace(logger,"Reduccion local OK -> %s generado",nombreReduccionLocal);
    send_EXITO_OPERACION(socket_cliente);
    exit(0);
};

