/*
 * md5_interface.c
 */

#include <stdio.h>

void md5_interface(char **comando){
	char *rutaTemp = "/home/utnso/archivoTemp.bin";

	char *contenidoRecibido = leerContenidoArchivo(comando[1]);
		if(string_equals_ignore_case(contenidoRecibido, "Error")){
			puts("Error al intentar leer el archivo (md5)");
		}
		else{
			FILE *archivo;
			archivo = fopen(rutaTemp, "wb+");
			fwrite(contenidoRecibido, strlen(contenidoRecibido)*sizeof(char),1, archivo);

			free(contenidoRecibido); // TODO: No esta haciendo el free
			fclose(archivo);


			system(string_from_format("md5sum %s", rutaTemp));

			if(remove(rutaTemp) == -1){
				//No se elimino
			}
		}
};
