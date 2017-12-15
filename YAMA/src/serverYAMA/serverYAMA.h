/*
 * ============================================================================
 * serverMaster.h
 * ============================================================================
 */

#ifndef SERVERMASTER_H_
#define SERVERMASTER_H_

// Inicia un servidor para atender las solicitudes de los procesos Master
//  en el puerto pasasdo por parametro (int)
void init_serverYAMA(int puerto);
void agregarAListado(int socket);
void eliminarMaster(int masterID);
int getMasterID(int socketMaster);
extern t_yama* configYAMA;
#endif /* SERVERMASTER_H_ */
