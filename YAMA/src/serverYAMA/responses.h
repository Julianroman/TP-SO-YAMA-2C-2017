/*
 * responses.h
 *
 *  Created on: 17/9/2017
 *      Author: utnso
 */

#ifndef RESPONSES_RESPONSES_H_
#define RESPONSES_RESPONSES_H_

#include <commons/collections/dictionary.h>
#include <utilidades/protocol/types.h>
#include <sys/types.h>
#include <utilidades/protocol/senders.h>
#include "../YAMA.h"
#include "../Planificador.h"


// Ejecutor
void responder_SOLICITUD(int socket, void*  data, HEADER_T tipoDeMensaje);

// Respuestas
void res_SOLICITUD_JOB(int socket,void*  data);
void res_RECIBIR_INFO(int socket,void*  data);

#endif /* RESPONSES_RESPONSES_H_ */
