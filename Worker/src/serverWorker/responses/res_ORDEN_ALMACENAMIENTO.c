/*
 * ============================================================================
 * res_orden_almacenamiento.c
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

void res_ORDEN_ALMACENAMIENTO(int socket_cliente,HEADER_T header,void* data){
	log_info(logger, "Respondiendo ORDEN_ALMACENAMIENTO");
    srand((unsigned)time(NULL));
    int random = rand() % 3;
    sleep(random);
	send_EXITO_OPERACION(socket_cliente);
};
