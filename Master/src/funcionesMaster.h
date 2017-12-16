/*
 * funcionesMaster.h
 *
 *  Created on: 15/11/2017
 *      Author: utnso
 */

#ifndef FUNCIONESMASTER_H_
#define FUNCIONESMASTER_H_

void leerConfiguracion(char* path, char** ipYama,int* puertoYama,int* backlogTransformacion);
char* scriptToChar(char* path);
void tituloMaster();
STATUS_MASTER recibirJob(int socketYama, int* masterID);
char *leerScript(int size, int nodeFD);
int getScritptSize(char * rutaTransformador);
char * inputGet();

#endif /* FUNCIONESMASTER_H_ */
