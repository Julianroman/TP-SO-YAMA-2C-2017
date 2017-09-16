/*
 * Sockets.h
 *
 *  Created on: 4/6/2017
 *      Author: utnso
 */

#ifndef SOCKETS_H_
#define SOCKETS_H_


void servidor(int puerto);

void cliente(const char* ip, int puerto, int id_tipo_proceso);

char* tipo_proceso(int id_tipo_proceso);
void enviar_archivo(int socketCliente, char* file);
void recibir_archivo(int socketCliente);


#endif /* SOCKETS_H_ */
