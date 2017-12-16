/*
 * info_interface.c
 */

#include <stdio.h>

void info_interface(char **comando){
	int cant = 0;
	while(comando[cant] != NULL){
		cant++;
	}

	if(cant != 2){
		fprintf(stderr, "Comando erroneo. Podria probar con: info [path_archivo] \n");
	}else{
		getInfoArchivo(comando[1]);
	}

};
