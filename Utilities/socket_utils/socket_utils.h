/*
 ============================================================================
 Nombre        : socket_utils.h
 Descripcion   :
 ============================================================================
 */
#ifndef SOCKET_UTILS_H_
#define SOCKET_UTILS_H_

/*
 ============================================================================
 Crea un socket y lo conecta a al servidor indicado
    Recicibe:
        ip      : ip del servidor en formato de cadena
        puerto  : puerto al cual conectarse en formato cadena
    Devuelve:
        retorno : file descriptor del socket creado en formato entero
                * (-1) en caso de error
 ============================================================================
*/
int crear_conexion(const char* ip,const char* puerto);

/*
 ============================================================================
 Crea un socket listener
    Recicibe:
        puerto  : puerto al cual se debe escuchar
        backlog : cantidad de conexiones simultaneas
    Devuelve:
        retorno : file descriptor del socket creado en formato entero
                * (-1) en caso de error
============================================================================
*/
int crear_listener(const char* puerto);

#endif /* SOCKET_UTILS_H_ */
