/*
 * Planificador.h
 *
 *  Created on: 20/10/2017
 *      Author: utnso
 */

#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_

t_list* jobsAPlanificar;

typedef struct {

	int numero;
	char *ip;
	char *puerto;
	int carga;
	t_list* bloques;
	int disponibilidad;
	int activo;
	int cantTareasHistoricas;

} t_nodo;

t_list* listaNodos;

#endif /* PLANIFICADOR_H_ */
