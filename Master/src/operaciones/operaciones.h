/*
 * operaciones.h
 *
 *  Created on: 21/10/2017
 *      Author: utnso
 */

#ifndef OPERACIONES_OPERACIONES_H_
#define OPERACIONES_OPERACIONES_H_

typedef enum {EXITO,DESCONECTADO} STATUS_MASTER;


STATUS_MASTER transformacion (int socketYAMA, payload_INFO_TRANSFORMACION* data);
STATUS_MASTER reduccionLocal (int socketYAMA, payload_INFO_REDUCCIONLOCAL* data);
STATUS_MASTER reduccionGlobal(int socketYAMA, void* data);
STATUS_MASTER almacenamiento (int socketYAMA, void* data);

#endif /* OPERACIONES_OPERACIONES_H_ */
