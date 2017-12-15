/*
 * cpto_interface.c
 */

#include <stdio.h>

void cpto_interface(char **comando){
	int cant = 0;
	while(comando[cant] != NULL){
		cant++;
	}

	if(cant != 3){
		fprintf(stderr, "Comando erroneo. Podria probar con: cpto [path_archivo_yamafs] [directorio_filesystem]");
	}else{

		char *contenidoRecibido = leerContenidoArchivo(comando[1]);
			if(string_equals_ignore_case(contenidoRecibido, "Error")){
				perror("Error al leer el archivo");
			}
			else{
				FILE *archivo;
				archivo = fopen(comando[2], "w");
				fwrite(contenidoRecibido, strlen(contenidoRecibido)*sizeof(char),1, archivo);

				free(contenidoRecibido);
				fclose(archivo);

				printf("El archivo fue almacenado con exito en: %s", comando[2]);
			}


	}




};
