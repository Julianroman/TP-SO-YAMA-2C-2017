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

typedef struct{
	char** array;
	int index;
} t_temporal;

void t_temporal_destroyer(void* victima){
	t_temporal* temporal = victima;
	free(temporal -> array);
	free(temporal);
}

void res_ORDEN_REDUCCIONGLOBAL(int socket_cliente,HEADER_T header,void* data){

	log_info(logger, "Respondiendo ORDEN_REDUCCIONGLOBAL");

	payload_ORDEN_REDUCCIONGLOBAL* orden = data;
	payload_TEMPORAL* temporalPayload;
	pid_t pid = getpid();
	int socketSubordinado;
	t_list* listaTemporales = list_create();
	char* nombreReduccionGlobal = orden -> nombreTemporal_ReduccionGlobal;

	// Cargar el temporal local
	char* temporalPath = string_from_format("tmp/%s",orden -> nombreTemporal_ReduccionLocal);
	char* temporalContents = temporalToChar(temporalPath);
	free(temporalPath);

	t_temporal* temporal = malloc(sizeof(t_temporal));
	temporal -> array = string_split(temporalContents, "\n");
	temporal -> index = 0;
	list_add(listaTemporales,temporal);


	// Recibir todas las instrucciones
	// Hasta que termine la lista
	while(header == ORDEN_REDUCCIONGLOBAL){
		log_info(logger, "Conectando a Worker en %s:%d",orden->IP_Nodo,orden->PUERTO_Nodo);

		// Enviar peticion de temporal
		socketSubordinado = crear_conexion(orden->IP_Nodo,orden->PUERTO_Nodo);
		send_PETICION_TEMPORAL(socketSubordinado, orden ->nombreTemporal_ReduccionLocal);

		// Recibir
		temporalPayload = receive(socketSubordinado,&header);
		temporal = malloc(sizeof(t_temporal));
		temporal -> index = 0;

		// Dividir
		temporal -> array = string_split(temporalPayload -> contenido, "\n");

		// Destruir payload
		//destroy_TEMPORAL(temporalPayload);

		log_info(logger, "Temporal recibido");

		//Guardar
		list_add(listaTemporales,temporal);

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


    //PIPEO INTENSIFIES!!!
	int pipe_padreAHijo[2];
	int pipe_hijoAPadre[2];

	pipe(pipe_padreAHijo);
	pipe(pipe_hijoAPadre);

	int status;


	if ((pid=fork()) == 0 ){
		// ************* HIJO ************* //


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
		execve(path, argv, envp);
		exit(1);


		// *********** END HIJO ************ //
	}else{
		// ************* PADRE ************* //


		// Cierro lo que no necesito
		close( pipe_padreAHijo[0] );
		close( pipe_hijoAPadre[1] );


		// APAREO & ESCRITURA
		int running = 1;
		int comparaciones;
		int i;
		t_temporal* proximaEscritura;
		t_temporal* retador;
		char* retadorString;
		char* proximaEscrituraString;
		int indiceProximaEscritura;

		while (running){
			comparaciones = list_size(listaTemporales);
			// Si ya agotamos todos los archivos termina el apareo/escritura
			if(comparaciones == 0){running = 0;break;}

			// Selecciono el proximo a escribir
			for(i = 0; i < comparaciones ; i++){
				// Primero queda como proxima escritura
				if(i==0){
					proximaEscritura = list_get(listaTemporales,i);
					indiceProximaEscritura = i;
					break;
				}

				// Buscar el menor
				retador = list_get(listaTemporales,i);
				retadorString = (retador -> array)[retador -> index];
				proximaEscrituraString = (proximaEscritura -> array)[proximaEscritura -> index];
				if(strcmp(retadorString,proximaEscrituraString) <= 0){
					proximaEscritura = retador;
					indiceProximaEscritura = i;
				}
			}


			// Escribir al pipe
			proximaEscrituraString = (proximaEscritura -> array)[proximaEscritura -> index];
			write(pipe_padreAHijo[1],proximaEscrituraString,strlen(proximaEscrituraString));

			// Sacar los archivos que ya completaron su contenido
			proximaEscritura -> index = (proximaEscritura -> index) + 1;
			proximaEscrituraString = (proximaEscritura -> array)[proximaEscritura -> index];
			if(proximaEscrituraString == NULL){
				list_remove_and_destroy_element(listaTemporales, indiceProximaEscritura,t_temporal_destroyer);
			}

		}


		// Cierro pipe
		close( pipe_padreAHijo[1]);


		// Espero al hijo
		waitpid(pid,&status,0);


		// Creo un archivo
		char* temporalPath = string_from_format("tmp/%s",nombreReduccionGlobal);
		FILE* fd = fopen(temporalPath,"w+");
		free(temporalPath);


		// Leo de la pipe y escribo en el archivo
		char bufferTemp;
		while(0 != read( pipe_hijoAPadre[0], &bufferTemp, 1)){
			putc(bufferTemp, fd);
		}


		// Cierro todito
		close(pipe_hijoAPadre[0]);
		fclose(fd);
		remove(path);


		// Esito
		send_EXITO_OPERACION(socket_cliente);
		exit(EXIT_SUCCESS);

		// ********** END PADRE ************ //
	}


};
