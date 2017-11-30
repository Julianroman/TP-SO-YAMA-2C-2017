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

	log_info(logger, "Respondiendo ORDEN_REDUCCIONGLOBAL");

	payload_ORDEN_REDUCCIONGLOBAL* orden = data;
	payload_TEMPORAL* temporalPayload;
	pid_t pid = getpid();
	int socketSubordinado;
	t_list* listaTemporales = list_create();

	// Cargar el temporal local
	char* temporalPath = string_from_format("tmp/%s",orden -> nombreTemporal_ReduccionLocal);
	char* temporalContents = temporalToChar(temporalPath);
	free(temporalPath);

	char** splitedTemporal = string_split(temporalContents, "\n");
	list_add(listaTemporales,splitedTemporal);


	// Recibir todas las instrucciones
	// Hasta que termine la lista
	while(header == ORDEN_REDUCCIONGLOBAL){
		log_info(logger, "Conectando a Worker en %s:%d",orden->IP_Nodo,orden->PUERTO_Nodo);

		// Enviar peticion de temporal
		socketSubordinado = crear_conexion(orden->IP_Nodo,orden->PUERTO_Nodo);
		send_PETICION_TEMPORAL(socketSubordinado, orden ->nombreTemporal_ReduccionLocal);

		// Recibir
		temporalPayload = receive(socket_cliente,&header);

		// Dividir
		splitedTemporal = string_split(temporalPayload -> contenido, "\n");

		// Destruir payload
		destroy_TEMPORAL(temporalPayload);

		//Guardar
		list_add(listaTemporales,splitedTemporal);

		// Repeat
		orden = receive(socket_cliente,&header);
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
	payload_SCRIPT* script = receive(socket_cliente,&header);
	if(header != SCRIPT){
		log_error(logger,"Se esperaba un archivo");
		send_FRACASO_OPERACION(socket_cliente);
		exit(1);
	}
	log_info(logger,"Reductor recibido");

	// Guardo el script
	char* contenido = script -> contenido;
	char* path = string_from_format("scripts/reductor%d", pid);
    FILE *fp = fopen(path, "ab");
    if (fp != NULL)
    {
        fputs(contenido, fp);
        fclose(fp);
    }

    // Le otorgo permisos de ejecucion
    // la funcion chmod no me estaria funcionando pero el buen system siempre provee
    char* chmodComand = string_from_format("chmod 777 %s", path);
    system(chmodComand);
    free(chmodComand);

    // Ejecutar
    char* comando = string_from_format("./%s", path);
    system(comando);
    free(comando);

    // Borro el script
    remove(path);
    srand((unsigned)time(NULL));
    int random = rand() % 3;
    sleep(random);
	send_EXITO_OPERACION(socket_cliente);
	exit(EXIT_SUCCESS);

};
