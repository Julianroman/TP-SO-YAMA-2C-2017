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

	log_info(logger, "Respondiendo ORDEN_REDUCCIONLOCAL");

	pid_t pid = getpid();
    char* transformadoPath;
    t_transformado* transformado;
    t_list * listaTransformados = list_create();
    size_t len = 0;
	payload_ORDEN_REDUCCIONLOCAL* orden = data;
	HEADER_T cabecera;


	// Guardar el nombre final
	char* nombreReduccionLocal = string_from_format("%s",orden -> nombreTemporal_ReduccionLocal);


	// Generar el path
	transformadoPath = string_from_format("tmp/%s",orden->nombreTemporal_Transformacion);


	// Cargar nodo
    transformado = malloc(sizeof(t_transformado));
    if((transformado -> file = fopen(transformadoPath,"r")) == NULL) puts("NULL FILE");
    transformado -> lastLine = NULL;
    	// Recibir linea
    if(getline(&(transformado -> lastLine),&len,transformado -> file) != -1){
		// Agregarlo a la lista si tiene contenido
		list_add(listaTransformados,transformado);
    }

    // Destruir orden cuando no la uso
	destroy_ORDEN_REDUCCIONLOCAL(orden);

	// Recibir las distintas reducciones en el mismo nodo
	orden = receive(socket_cliente,&header);
	while(header == ORDEN_REDUCCIONLOCAL){

		// Generar el path
		transformadoPath = string_from_format("tmp/%s",orden->nombreTemporal_Transformacion);

		// Cargar nodo
	    transformado = malloc(sizeof(t_transformado));
	    if((transformado -> file = fopen(transformadoPath,"r")) == NULL) puts("NULL FILE");
	    transformado -> lastLine = NULL;
	    	// Recibir linea
	    if(getline(&(transformado -> lastLine),&len,transformado -> file) != -1){

			// Agregarlo a la lista si tiene contenido
			list_add(listaTransformados,transformado);
	    }


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
	// Destruyo el fin de lista
	// WARNING IMPLEMENTANDO


	// Recibir script
	payload_SCRIPT* script = receive(socket_cliente,&cabecera);
	if(cabecera != SCRIPT){
		log_error(logger,"Se esperaba un archivo");
		send_FRACASO_OPERACION(socket_cliente);
		exit(1);
	}


	log_info(logger,"Reductor recibido");

	// Guardo el script
	char* contenido = script -> contenido;
	char* scriptPath = string_from_format("scripts/reductor%d", pid);
    FILE *fp = fopen(scriptPath, "ab");
    if (fp != NULL)
    {
        fputs(contenido, fp);
        fclose(fp);
    }

    // Le otorgo permisos de ejecucion
    // la funcion chmod no me estaria funcionando pero el buen system siempre provee
    char* chmodComand = string_from_format("chmod 777 %s", scriptPath);
    system(chmodComand);



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
		execve(scriptPath, argv, envp);
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
		t_transformado* candidatoAEscribir;
		t_transformado* transformadoAEscribir;
		int indiceProximaEscritura;

		while (running){
			comparaciones = list_size(listaTransformados);
			// Si ya agotamos todos los archivos termina el apareo/escritura
			if(comparaciones == 0){running = 0;break;}

			// Selecciono el proximo a escribir
			for(i = 0; i < comparaciones ; i++){
				candidatoAEscribir = list_get(listaTransformados,i);
				if(i==0){
					transformadoAEscribir = candidatoAEscribir;
					indiceProximaEscritura = i;
					break;
				}
				if(strcmp(candidatoAEscribir -> lastLine, transformadoAEscribir-> lastLine) <= 0){
					transformadoAEscribir = candidatoAEscribir;
					indiceProximaEscritura = i;
				}
			}


			// Escribir al pipe
			write(pipe_padreAHijo[1],transformadoAEscribir->lastLine,strlen(transformadoAEscribir->lastLine));

			// Sacar los archivos que ya completaron su contenido
			if(getline(&(transformadoAEscribir -> lastLine),&len,transformadoAEscribir -> file) == -1){
				list_remove_and_destroy_element(listaTransformados, indiceProximaEscritura,t_transformado_destroyer);
			}
		}


		// Cierro pipe
		close( pipe_padreAHijo[1]);


		// Espero al hijo
		waitpid(pid,&status,0);


		// Creo un archivo
		char* temporalPath = string_from_format("tmp/%s",nombreReduccionLocal);
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
		remove(scriptPath);


		// Esito
		send_EXITO_OPERACION(socket_cliente);
		exit(EXIT_SUCCESS);

		// ********** END PADRE ************ //
	}


};

