/*
 * cpfrom_interface.c
 */

#include <stdio.h>
//cpfrom [path_archivo_origen] [directorio_yamafs]
void cpfrom_interface(char **comando){
	int cant = 0;
	while(comando[cant] != NULL){
		cant++;
	}

	if(cant != 5){
		fprintf(stderr, "Comando erroneo. Podria probar con: cpfrom [path_archivo_origen] [directorio_yamafs] [tipo]");
	}else{
		almacenarArchivo(comando[1], comando[2], comando[3], comando[4]);
	}

};
