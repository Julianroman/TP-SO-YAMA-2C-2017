/*
 * etapas.h
 *
 *  Created on: 5/10/2017
 *      Author: utnso
 */

#ifndef ETAPAS_ETAPAS_H_
#define ETAPAS_ETAPAS_H_

typedef enum {EXITO} MASTER_STATUS;


/*
 *
 * Etapas del master
 * Parametros:
 *     (int)    socketYAMA : FD de un socket con el cual comunicarse con el YAMA
 *     #Depricated#(t_log*) logs       : Un puntero de logs para documentar todos los sucesos
 */
MASTER_STATUS etapa_transformacion (int socketYAMA, char* yamafs_archivo);
MASTER_STATUS etapa_reduccionlocal (int socketYAMA);
MASTER_STATUS etapa_reduccionglobal(int socketYAMA);
MASTER_STATUS etapa_almacenamiento (int socketYAMA);

#endif /* ETAPAS_ETAPAS_H_ */
