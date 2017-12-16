/*
 * rename_interface.c
 */
#include <stdio.h>

void rename_interface(char** comando){
	int cant = 0;
	while(comando[cant] != NULL){
		cant++;
	}

	if(cant != 3){
		fprintf(stderr, "Comando erroneo. Podria probar con: rename [path_original] [nombre_final] \n");
	}else{
		char **pathSplit = string_split(comando[1], "/");
		int cant = 0;
		while(pathSplit[cant] != NULL){
			cant ++;
		}
		int indice = findDirByname(pathSplit[cant - 1]);

		if(indice != -1){
			//Es un directorio
			printf("%s", comando[1]);
			if(esRutaYamaFS(comando[1])==1){
				renameDirectory(indice, comando[2]);
			}

		}else{
			if(esRutaYamaFSConNombre(comando[1])==1){
				indice = findDirByname(pathSplit[cant - 2]);
				char *ruta = string_from_format("root/metadata/archivos/%i/%s", indice, pathSplit[cant-1]);

				char *rutaNuevo = string_from_format("root/metadata/archivos/%i/%s", indice, comando[2]);


				if(rename(ruta, rutaNuevo) == -1){
					printf("No se pudo renombrar o no se encontro el archivo");
				}
				free(ruta);
				free(rutaNuevo);
			}

		}
		free(pathSplit);
	}


};

