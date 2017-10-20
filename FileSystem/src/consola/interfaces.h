/*
 * interface.h
 *
 *  Created on: 31/8/2017
 *      Author: utnso
 */

#ifndef INTERFACES_H_
#define INTERFACES_H_



// Interfaces a los distintos comandos
// 		A cada comando que entienda la consola se le corresponde
//		una funcion:
//			<denominacion del comando>_interface()
//		Encargada de realizar la llamada a la funcion del filesystem
//		que corresponda a la accion deseada

void format_interface();
void rm_interface();
void rename_interface();
void mv_interface();
void cat_interface();
void mkdir_interface();
void cpfrom_interface();
void cpto_interface();
void cpblock_interface();
void md5_interface();
void ls_interface();
void info_interface();

// Macheo comando/interfaz
// 		buscar_interfaz recibe un char* con la denominacion del comando
//		y devuelve un puntero a la funcion interface correspondiente
void* buscar_interfaz(char* comando);



#endif /* INTERFACES_H_ */
