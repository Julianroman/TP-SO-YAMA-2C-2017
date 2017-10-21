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
#include "../../YAMA.h"


// Ejecutor
YAMA_STATUS responder_SOLICITUD(int socket,HEADER_T tipoDeMensaje, void*  data, t_dictionary* diccionario);

// Respuestas
YAMA_STATUS res_SOLICITUD_PROCESAMIENTO   (int socket,void*  data, t_dictionary* diccionario);
YAMA_STATUS res_SOLICITUD_REDUCCIONLOCAL  (int socket,void*  data, t_dictionary* diccionario);
YAMA_STATUS res_SOLICITUD_REDUCCIONGLOBAL (int socket,void*  data, t_dictionary* diccionario);
YAMA_STATUS res_SOLICITUD_ALMACENAMIENTO  (int socket,void*  data, t_dictionary* diccionario);


#endif /* RESPONSES_RESPONSES_H_ */
