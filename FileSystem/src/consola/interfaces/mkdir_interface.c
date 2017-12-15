#include <stdio.h>

void mkdir_interface(char **comando){
	int cant = 0;
	while(comando[cant] != NULL){
		cant++;
	}

	if(cant != 2){
		fprintf(stderr, "Comando erroneo. Podria probar con: mkdir [path_directorio]");
	}else{
		createLogicDirectory(comando[1]);
	}


};
