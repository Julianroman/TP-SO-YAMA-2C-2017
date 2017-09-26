/*
 * responses.h
 *
 *  Created on: 17/9/2017
 *      Author: utnso
 */

#ifndef RESPONSES_RESPONSES_H_
#define RESPONSES_RESPONSES_H_
#include <utilidades/protocolo.h>

// Tipo de dato de uso interno del yama
// las respuestas deberan retornar un YAMA_STATUS
// para poder tomar las medidas necesarias en caso de error
typedef enum {EXITO} YAMA_STATUS;

// Ejecutor
YAMA_STATUS responder_solicitud(t_Mensaje tipoDeMensaje,int socket);

// Respuestas
YAMA_STATUS res_procesamiento(int socket);

#endif /* RESPONSES_RESPONSES_H_ */
