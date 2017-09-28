/*
 * responder_solicitudes.c
 *
 *  Created on: 17/9/2017
 *      Author: utnso
 */
#include "responses.h"
#include <utilidades/protocolo.h>
#include <stdio.h>

void responder_solicitud(t_Mensaje tipoDeMensaje){
	switch (tipoDeMensaje){
	case ORDENTRANS:
		res_orden_transformacion();
		break;
	case ORDENREDUXLOC:
		res_orden_reduxLocal();
		break;
	case ORDENREDUXGLOB:
		res_orden_reduxGlobal();
		break;
	case ORDENALMA:
		res_orden_almacenamiento();
		break;
	default:
		printf("Mensaje desconocido");
	}
};

