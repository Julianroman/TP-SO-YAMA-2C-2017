/*
 * etapas.h
 *
 *  Created on: 5/10/2017
 *      Author: utnso
 */

#ifndef ETAPAS_ETAPAS_H_
#define ETAPAS_ETAPAS_H_

#include <commons/log.h>

typedef enum {EXITO} MASTER_STATUS;

/*
 *
 * Etapas del master
 * Parametros:
 *     (int)    socketYAMA : FD de un socket con el cual comunicarse con el YAMA
 *     #Depricated#(t_log*) logs       : Un puntero de logs para documentar todos los sucesos  ## Es una variable global
 */
MASTER_STATUS etapa_transformacion (int socketYAMA,t_log* logger, char* yamafs_archivo);
MASTER_STATUS etapa_reduccionLocal (int socketYAMA,t_log* logger);
MASTER_STATUS etapa_reduccionGlobal(int socketYAMA,t_log* logger);
MASTER_STATUS etapa_almacenamiento (int socketYAMA,t_log* logger);

#endif /* ETAPAS_ETAPAS_H_ */
