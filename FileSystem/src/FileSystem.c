/*
 ============================================================================
 Name        : FileSystem.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <utilidades/Sockets.c>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <commons/log.h>
#include <commons/config.h>
#include <pthread.h>
#include "consola.h"
#include <sys/mman.h>
t_log* log;
int32_t miPuerto = 5040;
int32_t cantBloques = 20;
int32_t tamanioBloques = 1048576; //1MB
int32_t bitmap[20];
char* pathBitmap = "metadata/"; //Esto deberia estar en la carpeta metadata/bitmaps

int32_t estadoEstable = 0;
int32_t formateado = 0;
//

int validarArchivo(char* pathArchivo){
	FILE* archivo;
	archivo = fopen(pathArchivo,"r");
	if (archivo == NULL){
		//printf("No existe el archivo en el FileSystem\n");
			//fclose(archivo);
			return 0;
	}else{
			//printf("SI!!, Existe el archivo en el FileSystem\n");
			fclose(archivo);
			return 1;
	}
}
void enviarADataNode(char* map, int bloque, int tam, int size_bytes){

	printf("Enviado a data node: bloque %i -- bytes %i \n", bloque, size_bytes);
	//Falta el envio

}


int almacenarArchivo(char* location, char* destino, char* tipo){//Y también recibe "Los datos correspondientes"
	if(strcmp(tipo,"bin") == 0){
		puts("ARCHIVO BINARIO");
		FILE* file;
		if (!(file = fopen(location, "r"))){
			log_error(log, "El archivo no existe o no se pudo abrir");
		}
		else{
			//validar si el destino es valido
			int size_bytes;
			fseek(file,0,SEEK_END);
			size_bytes = ftell(file);
			rewind(file);

			int cant_bloques = (size_bytes/tamanioBloques) + (size_bytes % tamanioBloques != 0);
			int tam = 0;
			char* map;
			if((map = mmap(NULL, size_bytes, PROT_READ, MAP_SHARED, fileno(file),0)) == MAP_FAILED){
				log_error(log,"Error al mappear archivo\n");
			}
			int i;
			for(i = 0; i < cant_bloques; i++){
				if(size_bytes > tamanioBloques){
					enviarADataNode(map, i, tam, tamanioBloques);
					tam += tamanioBloques;
					size_bytes -= tamanioBloques;
				}else{
					size_bytes++;
					enviarADataNode(map, i, tam, size_bytes);
				}
			}
		}
		fclose(file);
		return 0;


	}
	else if(strcmp(tipo,"txt") == 0){
		puts("ARCHIVO DE TEXTO");
		FILE* file;
		if (!(file = fopen(location, "r"))){
			log_error(log, "El archivo no existe o no se pudo abrir");
		}
		else{
			//validar si el destino es valido
			int size_bytes;
			fseek(file,0,SEEK_END);
			size_bytes = ftell(file);
			rewind(file);

			int cant_bloques = (size_bytes/tamanioBloques) + (size_bytes % tamanioBloques != 0);
			int tam = 0;
			char* map;
			if((map = mmap(NULL, size_bytes, PROT_READ, MAP_SHARED, fileno(file),0)) == MAP_FAILED){
				log_error(log,"Error al mappear archivo\n");
			}
			int i = 0;
			map = strdup(map);
			//split de \n al map y le mando cada cosa al datanode
			char **str1 = string_split(map, "\n");
			char* text = string_new();
			char* textConcat = string_new();
			int32_t size_concat = 0;
			int bloq = 1;
			while (str1[i] != NULL)
			{

				textConcat = string_duplicate(text);
				string_append(&textConcat, str1[i]);
				string_append(&textConcat, "\n");
				size_concat = (strlen(textConcat) + 1) * sizeof(textConcat); //Tamaño en bytes

				if(size_concat < tamanioBloques){
					string_append(&text, str1[i]);
					tam += size_concat;

				}else{
					size_concat = (strlen(text) + 1) * sizeof(text);
					//printf("%s\n",text);
					enviarADataNode(text, bloq, tam, size_concat);
					bloq ++;
					text = string_new();
				}
				i++;

			}
			if(!string_is_empty(text)){
				size_concat = (strlen(text) + 1) * sizeof(text);
				//printf("%s\n",text);
				enviarADataNode(text, bloq, tam, size_concat);
				bloq ++;
			}
			free(str1);
			free(map);
			free(text);
		}
				fclose(file);

				return 0;
	}
	return 1;
}

void formatear(){
	/*if(existeUnEstadoAnterior){
		restaurarDesdeAhi
	}*/


	estadoEstable = 1;
	formateado = 1;
}

void crearBitmap(char* pathArchivo) {
	FILE* archivoBitmap;
	archivoBitmap = fopen(pathArchivo,"w");
	int i;

	for (i = 0; i < cantBloques; i++) {
		fwrite("0", 1, 1, archivoBitmap);
		bitmap[i] = 0;
	}
	fclose(archivoBitmap);
}

void inicializarBitmap(char* nombreNodo) {
	char *pathNewBitmap = string_new();
	strcpy(pathNewBitmap, pathBitmap);
	string_append(&pathNewBitmap, nombreNodo);
	string_append(&pathNewBitmap, ".dat");

	if(validarArchivo(pathNewBitmap) == 1){

		FILE* archivoBitmap;
		archivoBitmap = fopen(pathNewBitmap,"r");

		fgets(bitmap, sizeof(bitmap), archivoBitmap);
		bitmap[strlen(bitmap)] = '\0';
	}else{
		crearBitmap(pathNewBitmap);
	}

}
char* mapearArchivo(char *pathArchivo){
	struct stat estado_archivo;
	int fd=open(pathArchivo,R_OK|W_OK);
	char *archivoMapeado;
	if(fd<0){
		perror("open");
		exit(1);
	}
	if(fstat(fd,&estado_archivo)<0){
		perror("fstat");
		exit(1);
	}
	archivoMapeado=mmap(NULL,estado_archivo.st_size,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);
	close(fd);
	return archivoMapeado;
}
int main(int arg, char** argv) {
	log = log_create("fileSystem.log", "FileSystem", true, LOG_LEVEL_TRACE);
	log_trace(log, "Comienza el proceso FileSystem");


	if (argv[1] != NULL && strcmp(argv[1], "--clean")){
		log_info(log,"Iniciar ignorando/eliminando estado anterior");
		//format_fs(configFS,directorios);
	}
	else{
		/*restablecerEstado();
		for(int i = 0; i < list_size(archivos); i++){
			t_arch* archivo = list_get(archivos, i);
			printf("%d\n", archivo->padre);
		}*/
	}

	//servidor(miPuerto);

	//init_consola();

	///Creo el hiloConsola que llama a la funcion init_consola()
	/*pthread_t hiloConsola;
	pthread_create(&hiloConsola, NULL, (void*) init_consola, NULL);

	//Creo el hiloServidor que llama a la funcion servidor(miPuerto)
	pthread_t hiloServidor;
	pthread_create(&hiloServidor, NULL, (void*) servidor, miPuerto);

	//El proceso no termina hasta que mueren los dos hilos
	pthread_join(hiloConsola, NULL);
	pthread_join(hiloServidor, NULL);*/

	//Para las conexiones, mas adelante falta agregar que si
	//estadoEstable == 0
	//No permita conexiones de Workers o YAMA




	inicializarBitmap("DataNode1");
	//almacenarArchivo("Nodo1.bin","","bin");
	almacenarArchivo("Nodo10.txt","","txt");
	//importarArchivo("Nodo1.bin","");
	return EXIT_SUCCESS;
}
