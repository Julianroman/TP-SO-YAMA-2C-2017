#include <stdio.h>

void mkdir_interface(char **comando){
	int cant = 0;
	while(comando[cant] != NULL){
		cant++;
	}

	if(cant != 1){
		fprintf(stderr, "Comando erroneo. Podria probar con: mkdir [path_directorio]");
	}else{
		createDirectory(comando[1]);
	}


};
