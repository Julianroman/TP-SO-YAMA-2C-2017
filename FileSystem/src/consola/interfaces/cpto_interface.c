/*
 * cpto_interface.c
 */

#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

extern char *lecturaPathMD5;

void cpto_interface(char **comando){
	int cant = 0;
	while(comando[cant] != NULL){
		cant++;
	}

	if(cant != 3){
		fprintf(stderr, "Comando erroneo. Podria probar con: cpto [path_archivo_yamafs] [directorio_filesystem]");
	}else{

		if(leerContenidoArchivo(comando[1]) == 1){
			FILE *archvLectura = fopen(lecturaPathMD5, "r");
			if(archvLectura){
				fseek(archvLectura,0,SEEK_END);
				int sizeArchivo = ftell(archvLectura);
				rewind(archvLectura);


				char* lectura = malloc(sizeArchivo);

				char * map;
				if((map = mmap((caddr_t)0, sizeArchivo, PROT_READ, MAP_SHARED, fileno(archvLectura), 0)) == MAP_FAILED){
					perror("No se pudo mappear archivo");
				}
				memcpy(lectura, map, sizeArchivo);
				if (munmap(map, sizeArchivo) == -1)
				{
					perror("No se pudo liberar el map");
				}

				fclose(archvLectura);
				remove(lecturaPathMD5);


				char **arrayYamaFs = string_split(comando[1], "/");
				int cant = 0;
				while(arrayYamaFs[cant] != NULL ){
					cant++;
				}
				//El nombre es arrayYamaFs[cant-1]
				char *pathCompletoDest = string_new();
				if(string_ends_with(comando[2], "/"))
					pathCompletoDest = string_from_format("%s%s",comando[2],arrayYamaFs[cant-1]);
				else
					pathCompletoDest = string_from_format("%s/%s",comando[2],arrayYamaFs[cant-1]);
				FILE *archivo;
				archivo = fopen(string_from_format(pathCompletoDest), "w+");
				ftruncate(fileno(archivo),sizeArchivo);
				char * mapEscritura;
				if((mapEscritura = mmap((caddr_t)0, sizeArchivo, PROT_WRITE, MAP_SHARED, fileno(archivo), 0)) == MAP_FAILED){
					perror("No se pudo mappear archivo");
				}
				memcpy(mapEscritura, lectura, sizeArchivo);
				free(lectura);
				if (msync(mapEscritura, sizeArchivo, MS_SYNC) == -1)
				{
					perror("No se pudo sincronizar el archivo en el disco.");
				}
				if (munmap(mapEscritura, sizeArchivo) == -1)
				{
					perror("No se pudo liberar el map");
				}
				fclose(archivo);


				printf("El archivo fue almacenado con exito en: %s", pathCompletoDest);
				free(pathCompletoDest);
			}

		}
		else{
			perror("No se pudo concretar la operacion");
		}

	}




};
