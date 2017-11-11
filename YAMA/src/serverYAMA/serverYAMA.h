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
void agregarADiccionarioMaster(int idUltimoMasterCreado, int socket);

#endif /* SERVERMASTER_H_ */
