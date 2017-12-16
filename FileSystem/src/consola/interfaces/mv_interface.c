/*
 * mv_interface.c
 */

#include <stdio.h>

void mv_interface(char** comando){
	printf("comando <mv> reconocido / Implementacion en proceso\n");
/*	int cant = 0;
	while(comando[cant] != NULL){
		cant++;
	}

	if(cant != 3){
		fprintf(stderr, "Comando erroneo. Podria probar con: mv [path_original] [path_final] \n");
	}else{
		char **pathSplit = string_split(comando[1], "/");
		int cant = 0;
		while(pathSplit[cant] != NULL){
			cant ++;
		}
		int indice = findDirByname(pathSplit[cant - 1]);

		char **pathFinal = string_split(comando[2], "/");
		int cantFinal = 0;
		while(pathFinal[cant] != NULL){
			cantFinal ++;
		}
		int indiceFinal = findDirByname(pathFinal[cantFinal - 1]);


		if(indice != -1){
			//Es un directorio
			printf("%s", comando[1]);
			if(esRutaYamaFS(comando[1])==1 && esRutaYamaFS(comando[2])==1){
				moveDirectory(comando[1], comando[2]);

			}

		}else{
			if(esRutaYamaFSConNombre(comando[1])==1 && esRutaYamaFSConNombre(comando[1])==1){
				indice = findDirByname(pathSplit[cant - 2]);
				indiceFinal = findDirByname(pathSplit[cantFinal - 2]);

				char *ruta = string_from_format("root/metadata/archivos/%i/%s", indice, pathSplit[cant-1]);

				char *rutaNuevo = string_from_format("root/metadata/archivos/%i/%s", indiceFinal, pathFinal[cantFinal - 1]);


				system(string_from_format("mv %s %s", ruta, rutaNuevo));

				free(ruta);
				free(rutaNuevo);
			}

		}
		free(pathSplit);
	}*/
};
