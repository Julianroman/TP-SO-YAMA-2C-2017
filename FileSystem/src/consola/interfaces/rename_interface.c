/*
 * rename_interface.c
 */
#include <stdio.h>

void rename_interface(char** comando){
	if(rename(comando[1], comando[2]) == 0)
	{
		printf("%s fue renombrado a %s.\n", comando[1], comando[2]);
	}
	else
	{
		fprintf(stderr, "Error al intentar renombrar %s.\n", comando[1]);
	}
};

