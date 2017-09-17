/*
 * protocol_utils.h
 *
 *  Created on: 15/9/2017
 *      Author: utnso
 */

#ifndef UTILIDADES_PROTOCOL_UTILS_H_
#define UTILIDADES_PROTOCOL_UTILS_H_

#include "protocolo.h"


typedef struct{
	t_Mensaje estimulo;
	void*     accion;
}t_Respuesta;

// Dado un socket, recibe el header de un mensaje enviado
t_Mensaje recibir_header(int socket);

// Dados un tipo de mensaje y una tabla de acciones
// ejecuta la accion correspondiente al tipo de mensaje dado
//		nota: se ejecuta luego de recibir_header(1), y cada accion
//            tiene la responsabilidad de manejar su payload

void responder_solicitud(t_Mensaje tipoDeMensaje, t_Respuesta* tablaDeAcciones);

#endif /* UTILIDADES_PROTOCOL_UTILS_H_ */
