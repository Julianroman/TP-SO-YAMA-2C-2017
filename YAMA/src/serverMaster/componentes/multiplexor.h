/*
 * multiplexor.h
 *
 *  Created on: 27/9/2017
 *      Author: utnso
 */

#ifndef SERVERMASTER_COMPONENTES_MULTIPLEXOR_H_
#define SERVERMASTER_COMPONENTES_MULTIPLEXOR_H_

/*
 ============================================================================
 Recibe mensajes y atiende nuevas conexiones al servidorMaster del YAMA
    Recicibe:
        (int) listener              : File decriptor del socket sobre el que se van a escuchar nuevas conexiones
        (int) backlog               : Cantidad de conexiones en simultaneo
        (YAMA_STATUS (*func)(t_Mensaje,int,t_dictionary*)...
        ...   respuesta             : Funcion que mapea el header recibido con el procedimiento correspondiente
        (t_dictionary*) diccionario : Diccionario interno del YAMA que contiene informacion sobre los distintos clientes (Masters)

    Devuelve:
        retorno : file descriptor del socket creado en formato entero
                * (-1) en caso de error
 ============================================================================
*/
void multiplexar(int listener,int backlog,YAMA_STATUS (*respuesta)(t_Mensaje,int,t_dictionary*),t_dictionary* diccionario);


#endif /* SERVERMASTER_COMPONENTES_MULTIPLEXOR_H_ */
