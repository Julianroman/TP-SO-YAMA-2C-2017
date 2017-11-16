/*
 * ============================================================================
 * res_orden_reduxLocal.c
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

extern t_log* logger;

void res_ORDEN_REDUCCIONLOCAL(int socket_cliente,HEADER_T header,void* data){
	pid_t pid = getpid();

	log_info(logger, "Respondiendo ORDEN_REDUCCIONLOCAL");
	payload_ORDEN_REDUCCIONLOCAL* orden;
	HEADER_T cabecera;

	payload_SCRIPT* script = receive(socket_cliente,&cabecera);
	if(cabecera != SCRIPT){
		log_error(logger,"Se esperaba un archivo");
		send_FRACASO_OPERACION(socket_cliente);
		exit(1);
	}

	log_info(logger,"Reductor recibido");
	char* contenido = script -> contenido;

	// Guardo el script
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

    // Ejecutar
    char* comando = string_from_format("./%s", path);
    system(comando);

    // Borro el script
    remove(path);
    srand((unsigned)time(NULL));
    int random = rand() % 3;
    sleep(random);
	send_EXITO_OPERACION(socket_cliente);
	exit(EXIT_SUCCESS);


};

