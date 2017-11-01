/*
 * rm_interface.c
 */

#include <stdio.h>

void rm_interface(char** comando){
	//TODO flags
	if(string_equals_ignore_case(comando[1], "-d")){
		deleteDirectory(comando[2]);
	}
	else if(string_equals_ignore_case(comando[1], "-b")){
		printf("Delete file");
	}
	else{

		//TODO Asi tambien borra directorios???
		if(remove(comando[1]) == 0){
			printf("El archivo o directorio %s fue eliminado.\n", comando[1]);
		}
		else{
			fprintf(stderr, "No se pudo eliminar el archivo %s.\n", comando[1]);
		}
	}
};
