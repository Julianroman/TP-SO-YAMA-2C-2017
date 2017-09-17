/*
 * responder_solicitudes.c
 *
 *  Created on: 17/9/2017
 *      Author: utnso
 */
#include "../responses.h"
#include <utilidades/protocolo.h>
#include <stdio.h>

void responder_solicitud(t_Mensaje tipoDeMensaje){
	switch (tipoDeMensaje){
	case SOLICITUDPROCESAMIENTO:
		respuesta_procesamiento();
		break;
	default:
		printf("Mensaje desconocido");
	}
};

