/*
 * consola.h
 *
 *  Created on: 31/8/2017
 *
 */

#ifndef CONSOLA_H_
#define CONSOLA_H_

// Inicia la consola
void init_consola();

// Dada una linea, ejecuta el comando correspondiente
// o muestra un error por pantalla
void ejecutar_comando(char* linea);

#endif /* CONSOLA_H_ */
