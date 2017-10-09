/*
 * serverChild.h
 *
 *  Created on: 28/9/2017
 *      Author: utnso
 */

#ifndef SERVERWORKER_COMPONENTES_SERVERCHILD_H_
#define SERVERWORKER_COMPONENTES_SERVERCHILD_H_

/*
 * Logica de los hijos del proceso serverMaster
 * Los procesos hijos se encargaran de cumplir con las solicitudes (ordenes)
 */

/*
 *  Logica central
 *  Dado el file desriptor del socket donde se ha conectado el master
 *  el hijo tendra la responsabilidad de cumplir sus solicitudes
 */
void init_child(int socket_cliente);

#endif /* SERVERWORKER_COMPONENTES_SERVERCHILD_H_ */
