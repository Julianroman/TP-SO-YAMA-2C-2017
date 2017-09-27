/*
 * responder_solicitudes.c
 *
 *  Created on: 17/9/2017
 *      Author: utnso
 */
#include "responses.h"
#include <utilidades/protocolo.h>
#include <commons/collections/dictionary.h>
#include <stdio.h>

YAMA_STATUS responder_solicitud(t_Mensaje tipoDeMensaje,int socket,t_dictionary* diccionario){
	YAMA_STATUS status;
	switch (tipoDeMensaje){
	case SOLICITUDPROCESAMIENTO:
		status = res_procesamiento(socket,diccionario);
		return status;
		break;
	default:
		printf("Mensaje desconocido");
	}
};

