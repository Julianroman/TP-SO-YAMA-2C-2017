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
YAMA_STATUS responder_SOLICITUD(int socket,HEADER_T tipoDeMensaje, void*  data);

// Respuestas
YAMA_STATUS res_SOLICITUD_JOB(int socket,void*  data,);
YAMA_STATUS res_SOLICITUD_INFO(int socket,void*  data,);

#endif /* RESPONSES_RESPONSES_H_ */
