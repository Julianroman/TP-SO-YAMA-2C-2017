/*
 * cat_interface.c
 */

#include <stdio.h>

void cat_interface(char **comando){

	int cant = 0;
	while(comando[cant] != NULL){
		cant++;
	}

	printf("comando <cat> reconocido / Implementacion en proceso\n");

	char *contenidoRecibido = leerContenidoArchivo(comando[1]);
	if(string_equals_ignore_case(contenidoRecibido, "Error") == 0){

	}
	else{
		FILE *archivo;
		archivo = fopen("/home/utnso/archivo.bin", "wb+");
		fwrite(contenidoRecibido, strlen(contenidoRecibido)*sizeof(char),1, archivo);

		free(contenidoRecibido); // TODO: No esta haciendo el free
		fclose(archivo);
	}



};
