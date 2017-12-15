/*
 * md5_interface.c
 */

#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

extern char *lecturaPathMD5;

void md5_interface(char **comando){
	int cant = 0;
	while(comando[cant] != NULL){
		cant++;
	}

	if(cant != 2){
		fprintf(stderr, "Comando erroneo. Podria probar con: md5 [path_archivo_yamafs]");
	}else{

		if(leerContenidoArchivo(comando[1])){
			system(string_from_format("md5sum %s | awk '{print \"MD5:\" $1}'", lecturaPathMD5));

			remove(lecturaPathMD5);
		}else{
			perror("No se pudo concretar la operacion");
		}



	}

};
