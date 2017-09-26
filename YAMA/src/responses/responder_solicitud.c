/*
 * responder_solicitudes.c
 *
 *  Created on: 17/9/2017
 *      Author: utnso
 */
#include "../responses.h"
#include <utilidades/protocolo.h>
#include <stdio.h>

YAMA_STATUS responder_solicitud(t_Mensaje tipoDeMensaje,int socket){
	YAMA_STATUS status;
	switch (tipoDeMensaje){
	case SOLICITUDPROCESAMIENTO:
		status = res_procesamiento(socket);
		return status;
		break;
	default:
		printf("Mensaje desconocido");
	}
};

