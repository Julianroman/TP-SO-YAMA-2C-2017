/*
 * rm_interface.c
 */

#include <stdio.h>

void rm_interface(char** comando){
	//TODO flags
	if(remove(comando[1]) == 0){
		printf("El archivo o directorio %s fue eliminado.\n", comando[1]);
	}
	else{
		fprintf(stderr, "No se pudo eliminar el archivo %s.\n", comando[1]);
	}
};
