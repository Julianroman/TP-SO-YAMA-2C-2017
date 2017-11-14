/*
 * Sockets.h
 *
 *  Created on: 4/6/2017
 *      Author: utnso
 */

#ifndef SOCKETS_H_
#define SOCKETS_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include <commons/config.h>
#include <commons/string.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

void servidor(int puerto);

void cliente(const char* ip, int puerto, int id_tipo_proceso);

char* tipo_proceso(int id_tipo_proceso);
void enviar_archivo(int socketCliente, char* file);
void recibir_archivo(int socketCliente);


#endif /* SOCKETS_H_ */
