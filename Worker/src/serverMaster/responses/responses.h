/*
 * ============================================================================
 * responses.h
 * ============================================================================
 */

#ifndef RESPONSES_H_
#define RESPONSES_H_

#include <utilidades/protocolo.h>

// Ejecutor
void responder_solicitud(t_Mensaje tipoDeMensaje);

// Respuestas
void res_orden_transformacion();
void res_orden_reduxLocal();
void res_orden_reduxGlobal();
void res_orden_almacenamiento();

#endif /* RESPONSES_H_ */
