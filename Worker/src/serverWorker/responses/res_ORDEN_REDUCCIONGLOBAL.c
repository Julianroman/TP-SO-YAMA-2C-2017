/*
 * ============================================================================
 * res_orden_reduxGlobal.c
 * ============================================================================
 */

#include <stdio.h>
#include <utilidades/protocol/senders.h>
#include <utilidades/protocol/types.h>
#include <utilidades/protocol/receive.h>
#include <utilidades/socket_utils.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <sys/wait.h>


extern t_log* logger;

char* temporalToChar(char* path){
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

void res_ORDEN_REDUCCIONGLOBAL(int socket_cliente,HEADER_T header,void* data){

	log_info(logger, "Respondiendo ORDEN DE REDUCCION GLOBAL");

	payload_ORDEN_REDUCCIONGLOBAL* orden = data;
	payload_BLOQUE* temporalPayload;
	pid_t pid = getpid();
	int socketSubordinado;
	char* nombreReduccionGlobal = orden -> nombreTemporal_ReduccionGlobal;

	// Cargar el temporal local
	char* temporalPaths = string_from_format("tmp/%s",orden -> nombreTemporal_ReduccionLocal);
	char* temporalPath;
	int i = 0;
	FILE* temporal;


	orden = receive(socket_cliente,&header);
	// Recibir todas las instrucciones
	// Hasta que termine la lista
	while(header == ORDEN_REDUCCIONGLOBAL){
		log_info(logger, "Conectando a Worker en %s:%d",orden->IP_Nodo,orden->PUERTO_Nodo);

		// Enviar peticion de temporal
		socketSubordinado = crear_conexion(orden->IP_Nodo,orden->PUERTO_Nodo);
		send_PETICION_TEMPORAL(socketSubordinado, orden ->nombreTemporal_ReduccionLocal);

		// Recibir
		temporalPayload = receive(socketSubordinado,&header);
		log_info(logger, "Temporal recibido");

		//Guardar
		temporalPath = string_from_format("tmp/received_temporal%d",i);
		temporal = fopen(temporalPath,"w+");
	    fwrite(temporalPayload -> contenido,1,temporalPayload ->tamanio_bloque, temporal);
	    log_info(logger, "Temporal guardado");

	    // Agregar temporales recibidos
	    string_append(&temporalPaths, " ");
	    string_append(&temporalPaths, temporalPath);
	    log_info(logger, "Path generado");

		// Destruir payload
		// destroy_TEMPORAL(temporalPayload);

		// Repeat
		fclose(temporal);
		orden = receive(socket_cliente,&header);
		i++;
	}


	// Aseguro que termino la lista
	if(header != FIN_LISTA){
		log_error(logger,"Se esperaba un fin de lista");
		send_FRACASO_OPERACION(socket_cliente);
		exit(1);
	}
	if (header == FIN_COMUNICACION){
		log_error(logger,"El cliente se ha desconectado");
		send_FRACASO_OPERACION(socket_cliente);
		exit(1);
	}


	// Recibir archivo reductor
	payload_BLOQUE* script = receive(socket_cliente,&header);
	if(header != BLOQUE){
		log_error(logger,"Se esperaba un archivo");
		send_FRACASO_OPERACION(socket_cliente);
		exit(1);
	}
	log_info(logger,"Reductor recibido");

	// Guardo el script
	char* contenido = script -> contenido;
	char* scriptPath = string_from_format("scripts/reductor%d", pid);
    FILE *fp = fopen(scriptPath, "w+");
    if (fp != NULL)
    {
    	fwrite(contenido,script ->tamanio_bloque,1,fp);
        fclose(fp);
    }

    // Le otorgo permisos de ejecucion
    // la funcion chmod no me estaria funcionando pero el buen system siempre provee
    char* chmodComand = string_from_format("chmod 777 %s", scriptPath);
    system(chmodComand);
    free(chmodComand);

    // Ejecutar la transformacion
    char* comandoReduccion = string_from_format("sort -m %s | ./%s > tmp/%s",temporalPaths,scriptPath,nombreReduccionGlobal);
    log_info(logger,"Comando: %s\n",comandoReduccion );
    system(comandoReduccion);

    log_trace(logger,"Reduccion global OK -> %s generado",nombreReduccionGlobal);
    send_EXITO_OPERACION(socket_cliente);
    exit(0);

};
