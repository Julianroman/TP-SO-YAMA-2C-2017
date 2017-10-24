/*
 * inicializacion.c
 *
 *  Created on: 23/10/2017
 *      Author: utnso
 */

#include <utilidades/protocol/types.h>
#include <utilidades/protocol/receive.h>
#include "operaciones.h"
#include <stdio.h>

extern int socketYAMA;
extern int transformacionesRestantes;
extern int reduccionesLocalesRestantes;
extern int masterID;
extern int operaciones;

STATUS_MASTER inicializacion(){
	HEADER_T header;
	payload_JOB* data = receive(socketYAMA,&header);
	if(header != JOB)return DESCONECTADO;
	payload_JOB* job = data;
	operaciones                 = job->cantidad_operaciones;
	transformacionesRestantes   = job->cantidad_operaciones;
	reduccionesLocalesRestantes = job->cantidad_operaciones;
	masterID                    = job->id_master;
	return EXITO;
}
