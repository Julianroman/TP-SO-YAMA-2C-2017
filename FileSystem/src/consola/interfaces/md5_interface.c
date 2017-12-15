/*
 * md5_interface.c
 */

#include <stdio.h>

void md5_interface(char **comando){
	int cant = 0;
	while(comando[cant] != NULL){
		cant++;
	}

	if(cant != 2){
		fprintf(stderr, "Comando erroneo. Podria probar con: md5 [path_archivo_yamafs]");
	}else{
		char *rutaTemp = "archivoTemp.txt";

		char *contenidoRecibido = leerContenidoArchivo(comando[1]);
		if(string_equals_ignore_case(contenidoRecibido, "Error")){
			puts("Error al intentar leer el archivo (md5)");
		}
		else{
			FILE *archivo;
			archivo = fopen(rutaTemp, "w");
			fwrite(contenidoRecibido, strlen(contenidoRecibido)*sizeof(char),1, archivo);

			free(contenidoRecibido); // TODO: No esta haciendo el free
			fclose(archivo);


			system(string_from_format("md5sum %s | awk '{print \"MD5:\" $1}'", rutaTemp));

			//if(remove(rutaTemp) == -1){
				//No se elimino
			//}
		}
	}


};
