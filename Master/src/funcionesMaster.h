/*
 * funcionesMaster.h
 *
 *  Created on: 15/11/2017
 *      Author: utnso
 */

#ifndef FUNCIONESMASTER_H_
#define FUNCIONESMASTER_H_

void leerConfiguracion(char* path, char** ipYama,int* puertoYama);
char* scriptToChar(char* path);
void tituloMaster();
STATUS_MASTER recibirJob(int socketYama, int* masterID);

#endif /* FUNCIONESMASTER_H_ */