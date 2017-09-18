/*
 * ============================================================================
 * protocol_utils.h
 * ============================================================================
 */

#ifndef UTILIDADES_PROTOCOL_UTILS_H_
#define UTILIDADES_PROTOCOL_UTILS_H_

#include "protocolo.h"

// Dado un socket, recibe el header de un mensaje enviado
t_Mensaje recibir_header(int socket);

#endif /* UTILIDADES_PROTOCOL_UTILS_H_ */
