/*
 * cat_interface.c
 */

#include <stdio.h>
extern char *lecturaPathMD5;

void cat_interface(char **comando){

	int cant = 0;
	while(comando[cant] != NULL){
		cant++;
	}


	if(cant != 2){
		fprintf(stderr, "Comando erroneo. Podria probar con: cat [path_archivo]");
	}else{

		if(leerContenidoArchivo(comando[1])){
			system(string_from_format("cat %s", lecturaPathMD5));

			remove(lecturaPathMD5);
		}else{
			perror("No se pudo concretar la operacion");
		}



	}



};
