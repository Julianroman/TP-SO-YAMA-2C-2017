/*
 * ============================================================================
 * protocol_utils.c
 * ============================================================================
 */

#include <stdlib.h>
#include "protocolo.h"
#include "protocol_utils.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>


t_Mensaje recibir_header(int socket){
	t_Mensaje header;
	recv(socket,&header,sizeof header, 0);
	return header;
};
