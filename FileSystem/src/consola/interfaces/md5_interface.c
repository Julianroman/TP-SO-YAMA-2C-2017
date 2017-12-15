/*
 * md5_interface.c
 */

#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include <commons/config.h>
#include <commons/string.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

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
			//FILE *archivo;
			//archivo = fopen(rutaTemp, "w");
			//fwrite(contenidoRecibido, strlen(contenidoRecibido)*sizeof(char),1, archivo);
			int size = strlen(contenidoRecibido)*sizeof(char);
			int archivo;


			archivo = fopen(rutaTemp,"w");
			ftruncate(fileno(archivo),size);
			fclose(archivo);

			archivo = open(rutaTemp, O_RDWR);

			char * map;
			if((map = mmap((caddr_t)0, size, PROT_WRITE, MAP_SHARED, archivo, 0)) == MAP_FAILED){
				printf("No se pudo mappear archivo");
			}
			memcpy(map, contenidoRecibido, strlen(contenidoRecibido)*sizeof(char));
			if (msync(map, strlen(contenidoRecibido)*sizeof(char), MS_SYNC) == -1)
			{
				printf("No se pudo sincronizar el archivo en el disco.");
			}
			if (munmap(map, size) == -1)
			{
				printf("No se pudo liberar el map");
			}
			close(archivo);

			free(contenidoRecibido); // TODO: No esta haciendo el free
			//fclose(archivo);


			system(string_from_format("md5sum %s | awk '{print \"MD5:\" $1}'", rutaTemp));

			//if(remove(rutaTemp) == -1){
				//No se elimino
			//}
		}
	}


};
