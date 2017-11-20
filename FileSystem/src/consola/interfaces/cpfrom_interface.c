/*
 * cpfrom_interface.c
 */

#include <stdio.h>
//cpfrom [path_archivo_origen] [directorio_yamafs]
void cpfrom_interface(char **comando){
	almacenarArchivo(comando[1], comando[2], comando[3], comando[4]);
};
