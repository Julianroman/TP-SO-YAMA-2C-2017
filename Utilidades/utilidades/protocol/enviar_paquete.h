/*
 * enviar_paquete.h
 *
 *  Created on: 2/10/2017
 *      Author: utnso
 */

#ifndef UTILIDADES_PROTOCOL_ENVIAR_PAQUETE_H_
#define UTILIDADES_PROTOCOL_ENVIAR_PAQUETE_H_

/*
 * enviar_paquete(3)
 * Dados:
 * 		socket : un socket al cual mandarle el paquete
 * 		paquete: un paquete (ya serializado y con header incluido)
 * 		tamanio_paquete: el tamaño del paquete a mandar
 * Envia el paquete por el socket
 */

void enviar_paquete(int socket, char * paquete, int tamanio_paquete);

#endif /* UTILIDADES_PROTOCOL_ENVIAR_PAQUETE_H_ */
