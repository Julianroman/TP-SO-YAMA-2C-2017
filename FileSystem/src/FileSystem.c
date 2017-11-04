/*
 ============================================================================
 Name        : FileSystem.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */


#include "consola.h"
#include "funcionesFS.h"

#include <stdio.h>
#include <string.h>

#define TOTALDIRECTORIOS 100
#define PATHDIRECTORIOS "/home/utnso/Directorios.txt"
#define PATHBITMAP "root/metadata/bitmaps/"

char* directorioRaiz = "root/";


t_log* log;
int32_t miPuerto = 5040;
int32_t cantBloques = 20;
int32_t tamanioBloques = 1048576; //1MB
int32_t bitmap[20];



int32_t estadoEstable = 0;
int32_t formateado = 0;

t_list* listaDeNodos;
//t_list* tablaDirectorios;
t_directory* tablaDeDirectorios;
//
void copiaLocalAlYamafs(char* pathOrigen, char* pathDestino){

	FILE* fileOrigen;
	if (!(fileOrigen = fopen(pathOrigen, "r"))){
		log_error(log, "El archivo no existe o no se pudo abrir");
	}
	else{
		//validar si el destino es valido
		int size_bytes;
		fseek(fileOrigen,0,SEEK_END);
		size_bytes = ftell(fileOrigen);
		rewind(fileOrigen);

		int cant_bloques = (size_bytes/tamanioBloques) + (size_bytes % tamanioBloques != 0);
		int tam = 0;
		char* map;
		if((map = mmap(NULL, size_bytes, PROT_READ, MAP_SHARED, fileno(fileOrigen),0)) == MAP_FAILED){
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
			size_concat = strlen(textConcat) * sizeof(char); //Tamaño en bytes

			if(size_concat < tamanioBloques){
				string_append(&text, str1[i]);
				tam += size_concat;

			}else{
				size_concat = strlen(text) * sizeof(char);
				//printf("%s\n",text);
				//enviarADataNode(text, bloq, tam, size_concat);

				char* pathConcat = malloc(sizeof(char*));
				pathConcat = string_duplicate(pathDestino);
				string_append(&pathConcat, "/arc");
				string_append(&pathConcat, string_itoa(bloq));
				string_append(&pathConcat, ".txt");

				FILE* fileDestino;
				fileDestino = fopen(pathConcat, "w");
				if (!(fileDestino = fopen(pathConcat, "w"))){
						log_error(log, "Error al intentar escribir el archivo");
				}else{
					if(fwrite(text, size_concat, 1, fileDestino) != 1){
						log_error(log, "Error al intentar escribir el archivo");
					}else{
						log_trace(log, "El archivo fue copiado correctamente");
					}
					fclose(fileDestino);
					free(pathConcat);

					bloq ++;
					text = string_new();
				}

			}
			i++;

		}
		if(!string_is_empty(text)){
			size_concat = strlen(text) * sizeof(char);
			//printf("%s\n",text);
			char* pathConcat = malloc(sizeof(char*));
			pathConcat = string_duplicate(pathDestino);
			string_append(&pathConcat, "/arc");
			string_append(&pathConcat, string_itoa(bloq));
			string_append(&pathConcat, ".txt");

			FILE* fileDestino;
			fileDestino = fopen(pathConcat, "w");
			if (!(fileDestino = fopen(pathConcat, "w"))){
					log_error(log, "Error al intentar escribir el archivo");
			}else{
				if(fwrite(text, size_concat, 1, fileDestino) != 1){
					log_error(log, "Error al intentar escribir el archivo");
				}else{
					log_trace(log, "El archivo fue copiado correctamente");
				}
				fclose(fileDestino);
				free(pathConcat);

				bloq ++;
			}


		}
		free(str1);
		free(map);
		free(text);
	}
	fclose(fileOrigen);


}
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
				size_concat = strlen(textConcat) * sizeof(char); //Tamaño en bytes

				if(size_concat < tamanioBloques){
					string_append(&text, str1[i]);
					tam += size_concat;

				}else{
					size_concat = strlen(text) * sizeof(char);
					//printf("%s\n",text);
					enviarADataNode(text, bloq, tam, size_concat);
					bloq ++;
					text = string_new();
				}
				i++;

			}
			if(!string_is_empty(text)){
				size_concat = strlen(text) * sizeof(char);
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

void almacenarBitmapEnArchivo(t_nodo *unNodo){
	//pathBitmap = malloc(1000);
	char* name = string_itoa(unNodo->nroNodo);
	char *pathNewBitmap = malloc(1000);
	strcpy(pathNewBitmap, PATHBITMAP);
	string_append(&pathNewBitmap, "Nodo");
	string_append(&pathNewBitmap, name);
	string_append(&pathNewBitmap, ".dat");

	FILE* archivoBitmap;
	archivoBitmap = fopen(pathNewBitmap,"w");

	int j;
	for (j = 0; j < cantBloques; j++) {
		bool a = bitarray_test_bit(unNodo->bitmap, j);
		if(a == 0){
			fwrite("0", 1, 1, archivoBitmap);
		}
		if(a == 1){
			fwrite("1", 1, 1, archivoBitmap);
		}
	}
	log_trace(log, "El bitmap fue almacenado en: %s", pathNewBitmap);
	fclose(archivoBitmap);
	free(pathNewBitmap);
}

int bloquesLibresEnNodo(t_nodo* unNodo){
	int cantidad = 0;

	int j;
	for (j = 0; j < cantBloques; j++) {
		bool a = bitarray_test_bit(unNodo->bitmap, j);
		if(a == 0){
			cantidad ++;
		}
	}
	return cantidad;
}

int cantidadTotalBloquesLibres(){ //TODO con el segundo da 18
	int cantidad;
	cantidad = 0;
	//PARA CADA ELEMENTO DE LA LISTA
	int i;
	for (i = 0; i < list_size(listaDeNodos); i ++){
		t_nodo *unNodo = malloc(sizeof(t_nodo));
		unNodo = list_get(listaDeNodos, i);
		cantidad += bloquesLibresEnNodo(unNodo);
		nodo_destroy(unNodo);
	}
	log_trace(log,"Total bloques libres: %d", cantidad);
	return cantidad;
}


void escribirBloqueLibre(t_nodo* unNodo,int bloque){

	//ESCRIBO LO QUE TENGA QUE ESCRIBIR Y DESPUES CAMBIO EL BITMAP Y LO ALMACENO
	bitarray_set_bit(unNodo->bitmap, bloque);
	almacenarBitmapEnArchivo(unNodo);
}
int proximoBloqueLibre(t_nodo* unNodo){
	int j;
	for (j = 0; j < tamanioBloques; j++) {
		bool a = bitarray_test_bit(unNodo->bitmap, j);
		if(a == 0){
			log_trace(log, "El proximo bloque libre es el: %d", j);
			return j;
		}
	}
	log_trace(log, "No hay bloques libres en el nodo indicado");
	return -1;
}
void printBitmap(t_bitarray* unBitarray) {
	int j;
	for (j = 0; j < tamanioBloques; j++) {
		bool a = bitarray_test_bit(unBitarray, j);
		log_info(log,"%i", a);
	}
	log_info(log,"\n");
}

t_bitarray* initOrCreateBitmap(int nroNodo){
	char* name = string_itoa(nroNodo);
	char *pathNewBitmap = malloc(1000);
	strcpy(pathNewBitmap, PATHBITMAP);
	string_append(&pathNewBitmap, "nodo");
	string_append(&pathNewBitmap, name);
	string_append(&pathNewBitmap, ".dat");

	t_bitarray* unBitarray;

	FILE* bitmap;
	if (!(bitmap = fopen(pathNewBitmap, "r"))){
		log_info(log, "El bitmap del nodo %i no existe. Se inicializara.", nroNodo);

		char* data[] = {00000000000000000000};

		unBitarray = bitarray_create_with_mode(data,3, MSB_FIRST);

		bitmap = fopen(pathNewBitmap, "wb");
		if (bitmap != NULL) {
			fwrite(unBitarray, (sizeof(t_bitarray)), 1, bitmap);
			fclose(bitmap);
		}

	}
	else{
		unBitarray = malloc(sizeof(t_bitarray));
		bitmap= fopen(pathNewBitmap, "rb");
		if (bitmap != NULL) {
			fread(bitmap, (sizeof(t_bitarray)), 1, bitmap);
			fclose(bitmap);
		}
		log_info(log, "El bitmap del nodo%i fue leido con exito.", nroNodo);
	}
	return unBitarray;
}

void inicializarNodo(int nroNodo){
	t_bitarray* unBitarray = initOrCreateBitmap(nroNodo);

	t_nodo* nuevoNodo;
	nuevoNodo = nodo_create(nroNodo, unBitarray);
	list_add(listaDeNodos, nuevoNodo);


}

void printLs(char* path){
	char **padres = string_split(path, "/");
	int cant;
	cant = strlen(padres) / sizeof(char*);

	int indice = findDirByname(padres[cant - 1]);
	if(indice == -1){
		log_error(log, "No se encontro el directorio");
	}
	else{
		int i;
		for (i = 0; i < TOTALDIRECTORIOS; i++) {
			if(tablaDeDirectorios[i].padre == indice){ //TODO faltan los archivos
				printf("%s ", tablaDeDirectorios[i].nombre );
			}

		}
		printf("\n");
	}

}

void printTablaDeDirectorios(){
	printf("\n");
	int i;
	for (i = 0; i < TOTALDIRECTORIOS; i++) {
		if(tablaDeDirectorios[i].indice != -1){
			printf("%i -- %s -- %i \n",tablaDeDirectorios[i].indice, tablaDeDirectorios[i].nombre, tablaDeDirectorios[i].padre);
		}

	}
}
void saveTablaDeDirectorios(){
	FILE* tabla;
	tabla = fopen(PATHDIRECTORIOS, "wb");
	if (tabla != NULL) {
		fwrite(tablaDeDirectorios, (sizeof(t_directory) * TOTALDIRECTORIOS), 1, tabla);
		fclose(tabla);
	}
	printTablaDeDirectorios();
}
void initTablaDeDirectorios(){
	FILE* tabla;
	if (!(tabla = fopen(PATHDIRECTORIOS, "r"))){

		tablaDeDirectorios = malloc(sizeof(t_directory) * TOTALDIRECTORIOS);
		log_info(log, "El archivo de directorios no existe. Se inicializara.");

		tablaDeDirectorios[0].indice = 0;
		strcpy(tablaDeDirectorios[0].nombre, "root");
		tablaDeDirectorios[0].padre = -1;
		int i;
		for (i = 1; i < TOTALDIRECTORIOS; i++) {
			tablaDeDirectorios[i].indice = -1;
			strcpy(tablaDeDirectorios[i].nombre, "");
			tablaDeDirectorios[i].padre = -1;
		}
		tabla = fopen(PATHDIRECTORIOS, "wb");
		if (tabla != NULL) {
			fwrite(tablaDeDirectorios, (sizeof(t_directory) * TOTALDIRECTORIOS), 1, tabla);
			fclose(tabla);
		}


		//fwrite(tablaDeDirectorios, (sizeof(t_directory) * TOTALDIRECTORIOS), 1, tabla);
	}
	else{
		//int fdDirectorios;
		//fdDirectorios = open(PATHDIRECTORIOS, "r");
		//tablaDeDirectorios = (t_directory*) mmap(0, sizeof(t_directory) * TOTALDIRECTORIOS, PROT_READ | PROT_WRITE, MAP_SHARED, fdDirectorios, 0);
		tablaDeDirectorios = malloc(sizeof(t_directory) * TOTALDIRECTORIOS);
		tabla= fopen(PATHDIRECTORIOS, "rb");
		if (tabla != NULL) {
			fread(tablaDeDirectorios, (sizeof(t_directory) * TOTALDIRECTORIOS), 1, tabla);
			fclose(tabla);
		}
		log_info(log, "La tabla de directorios fue leida con exito.");
	}
	printTablaDeDirectorios();


}

int findDirByname(char* name){
	int encontrado = -1;
	int i;
	for(i = 0; i < TOTALDIRECTORIOS; i++){
		if(strcmp(tablaDeDirectorios[i].nombre, name) == 0){ //strcmp devuelve 0 si son iguales
			encontrado = tablaDeDirectorios[i].indice;
		}
	}

	return encontrado;
}

void createDirectory(char* path){
	if(string_starts_with(path, "/")){
		log_error(log, "Error al crear directorio. Se encuentra en root/");
	}
	else{
		char* pathConcat = string_new();
			string_append(&pathConcat, directorioRaiz);
			string_append(&pathConcat, path);
			int indiceDisponible = -1;
			int j;
			for(j = (TOTALDIRECTORIOS - 1) ; j >= 0; j--){
				if(tablaDeDirectorios[j].indice == -1){ //TODO
					indiceDisponible = j;
				}
			}
			if(indiceDisponible == -1){
				log_error(log, "La tabla de directorios esta completa");
			}else{

				struct stat st = {0};

				if (stat(pathConcat, &st) == -1) { //Si no existe el path, lo creo
					if(mkdir(pathConcat, 0700) == 0){
						char **padres = string_split(path, "/");
						int cant;
						cant = strlen(padres) / sizeof(char*); //Length de padres
						if(cant == 1){
							tablaDeDirectorios[indiceDisponible].indice = indiceDisponible;
							strcpy(tablaDeDirectorios[indiceDisponible].nombre, padres[0]);
							tablaDeDirectorios[indiceDisponible].padre = 0;
						}
						else{
							int32_t father;
							log_trace(log,"Padre: %s", padres[cant-2]);
							if(strcmp(padres[cant-2], "root") == 0){
								father = 0;
							}else{
								father = findDirByname(padres[cant-2]);
							}

							tablaDeDirectorios[indiceDisponible].indice = indiceDisponible;
							strcpy(tablaDeDirectorios[indiceDisponible].nombre, padres[cant-1]);
							tablaDeDirectorios[indiceDisponible].padre = father;
						}

						log_trace(log, "El directorio %s fue creado con exito.", pathConcat);

					}else{
						log_error(log, "Error al crear directorio");
					}
				}
				else{
					log_error(log, "El directorio ya existe o no se pudo crear");
				}
			}
			saveTablaDeDirectorios();
	}


}

void deleteDirectory(char* path){
	if(string_starts_with(path, "/")){
		log_error(log, "Error al crear directorio. Se encuentra en root/");
	}else{
		char* pathConcat = string_new();
		string_append(&pathConcat, directorioRaiz);
		string_append(&pathConcat, path);
		//TODO
		struct stat st = {0};

		if (stat(pathConcat, &st) == -1) { //Si no existe el path, no lo elimino
			log_error(log, "El directorio no existe");
		}else{ //Faltaria verificar que sea ese con el padre (Pueden haber dos directorios que se llamen igual)
			char **padres = string_split(path, "/");
			int cant;
			cant = strlen(padres) / sizeof(char*);

			int indice = findDirByname(padres[cant-1]);
			tablaDeDirectorios[indice].indice = -1;
			strcpy(tablaDeDirectorios[indice].nombre, "");
			tablaDeDirectorios[indice].padre = -1;

			if(remove(path) == 0){
				printf("El directorio %s fue eliminado.\n", pathConcat);
			}
			else{
				fprintf(stderr, "No se pudo eliminar el archivo %s.\n", pathConcat);
			}
		}

		saveTablaDeDirectorios();
	}

}

int main(int arg, char** argv) {
	log = log_create("fileSystem.log", "FileSystem", true, LOG_LEVEL_TRACE);
	log_trace(log, "Comienza el proceso FileSystem");

	listaDeNodos = list_create();
	initTablaDeDirectorios();

	if (argv[1] != NULL && strcmp(argv[1], "--clean")){
		log_info(log,"Iniciar ignorando/eliminando estado anterior");
		//format_fs(configFS,directorios);
	}
	else{
		//log_info(log,"Iniciar reestableciendo desde estado anterior");
		/*restablecerEstado();
		for(int i = 0; i < list_size(archivos); i++){
			t_arch* archivo = list_get(archivos, i);
			printf("%d\n", archivo->padre);
		}*/
	}

	pthread_t hiloConsola;
	pthread_create(&hiloConsola, NULL, (void*) init_consola, NULL);

	pthread_t hiloServidor;
	pthread_create(&hiloServidor, NULL, (void*) servidor, miPuerto);

	pthread_join(hiloConsola, NULL);
	pthread_join(hiloServidor, NULL);

	//Para las conexiones, mas adelante falta agregar que si
	//estadoEstable == 0
	//No permita conexiones de Workers o YAMA

	//createDirectory("metadata");
	//createDirectory("metadata/bitmaps");

	/*inicializarNodo(2);
	inicializarNodo(1);
	cantidadTotalBloquesLibres();*/

	//createDirectory("some");
	//createDirectory("some/other");
	//createDirectory("ro");
	//createDirectory("some/carpeta"); //TODO con este rompe en el find

	//tablaDeDirectoriosEnArchivo();
	//createDirectory("some/dir")


	//almacenarArchivo("Nodo1.bin","","bin");
	//almacenarArchivo("Nodo10.txt","","txt");
	//importarArchivo("Nodo1.bin","");

	//copiaLocalAlYamafs("/home/utnso/Nuevo.txt", "root");


	return EXIT_SUCCESS;
}
