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

t_log* log;
int32_t miPuerto = 5040;
int32_t cantBloques = 20;
int32_t tamanioBloques = 1048576; //1MB
int32_t bitmap[20];

char* directorioRaiz = "root/";
char* pathBitmap = "root/metadata/bitmaps/";

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
	strcpy(pathNewBitmap, pathBitmap);
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

t_bitarray* inicializarBitmap() { //EN REVISIÓN
	char* data[] = {00000000000000000000};
	t_bitarray* unBitarray;
	unBitarray = bitarray_create_with_mode(data,sizeof(data), MSB_FIRST);

	int j=0;
	for(j=0; j < 24; j++){
		bitarray_clean_bit(unBitarray, j);
	}

	log_trace(log, "El bitmap fue inicializado correctamente");
	return unBitarray;
}

void inicializarNodo(int nroNodo){
	char* data[] = {00000000000000000000};
	t_bitarray* unBitarray;
	unBitarray = bitarray_create_with_mode(data,3, MSB_FIRST);

	t_nodo* nuevoNodo;
	nuevoNodo = nodo_create(nroNodo, unBitarray);
	almacenarBitmapEnArchivo(nuevoNodo);
	list_add(listaDeNodos, nuevoNodo);


}

void refreshTablaDeDirectorios(){
	FILE* tabla;
	if (!(tabla = fopen(PATHDIRECTORIOS, "r"))){


		tablaDeDirectorios = malloc(sizeof(t_directory) * TOTALDIRECTORIOS);
		log_info(log, "El archivo de directorios no existe. Se inicializara.");

		int i;
		for (i = 0; i < TOTALDIRECTORIOS; i++) {
			tablaDeDirectorios[i].indice = -1;
			tablaDeDirectorios[i].nombre = "";
			tablaDeDirectorios[i].padre = -1;
		}
		tabla = fopen(PATHDIRECTORIOS,"ab+");

		char* map;
		if((map = mmap(NULL, sizeof(t_directory) * TOTALDIRECTORIOS, PROT_READ | PROT_WRITE, MAP_SHARED, fileno(tabla),0)) == MAP_FAILED){
			log_error(log,"Error al mappear archivo\n");
		}
		else{

			memcpy(map, tablaDeDirectorios,(sizeof(t_directory) * TOTALDIRECTORIOS));
		}
		//fwrite(tablaDeDirectorios, (sizeof(t_directory) * TOTALDIRECTORIOS), 1, tabla);
	}
	else{
		int fdDirectorios;
		fdDirectorios = open(PATHDIRECTORIOS, "r");
		tablaDeDirectorios = (t_directory*) mmap(0, sizeof(t_directory) * TOTALDIRECTORIOS, PROT_READ | PROT_WRITE, MAP_SHARED, fdDirectorios, 0);
	}



}

int findDirByname(char* name, t_directory *tabla){
	int encontrado = 0;
	int i;
	for(i = 0; i < TOTALDIRECTORIOS; i++){
		if(strcmp(tabla[i].nombre, name) == 0){ //strcmp devuelve 0 si son iguales
			encontrado = tabla[i].indice;
		}
	}

	return encontrado;
}

void createDirectory(char* path){
	refreshTablaDeDirectorios();

	int indiceDisponible = -1;
	int j;
	for(j = (TOTALDIRECTORIOS - 1) ; j >= 0; j--){
		if(string_equals_ignore_case(tablaDeDirectorios[j].nombre, "") == 0){ //TODO
			indiceDisponible = j;
		}
	}
	if(indiceDisponible == -1){
		log_error(log, "La tabla de directorios esta completa");
	}else{

		struct stat st = {0};

		if (stat(path, &st) == -1) { //Si no existe el path, lo creo
			if(mkdir(path, 0700) == 0){
				char **padres = string_split(path, "/");
				int cant;
				cant = strlen(padres) / sizeof(char*); //Length de padres
				if(cant == 1){
					tablaDeDirectorios[indiceDisponible].indice = indiceDisponible;
					tablaDeDirectorios[indiceDisponible].nombre = padres[0];
					tablaDeDirectorios[indiceDisponible].padre = 0;
				}
				else{
					int32_t father;
					log_trace(log,"Padre: %s", padres[cant-2]);
					if(strcmp(padres[cant-2], "root") == 0){
						father = 0;
					}else{
						father = findDirByname(padres[cant-2], tablaDeDirectorios);
					}

					tablaDeDirectorios[indiceDisponible].indice = indiceDisponible;
					tablaDeDirectorios[indiceDisponible].nombre = padres[cant-1];
					tablaDeDirectorios[indiceDisponible].padre = father;
				}

				log_trace(log, "El directorio %s fue creado con exito.", path);

			}else{
				log_error(log, "Error al crear directorio");
			}
		}
		else{
			log_error(log, "El directorio ya existe o no se pudo crear");
		}
	}

}

int main(int arg, char** argv) {
	log = log_create("fileSystem.log", "FileSystem", true, LOG_LEVEL_TRACE);
	log_trace(log, "Comienza el proceso FileSystem");

	listaDeNodos = list_create();
	//tablaDirectorios = list_create();
	refreshTablaDeDirectorios();

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

	///Creo el hiloConsola que llama a la funcion init_consola()
	pthread_t hiloConsola;
	//pthread_create(&hiloConsola, NULL, (void*) init_consola, NULL);

	//Creo el hiloServidor que llama a la funcion servidor(miPuerto)
	pthread_t hiloServidor;
	//pthread_create(&hiloServidor, NULL, (void*) servidor, miPuerto);

	//El proceso no termina hasta que mueren los dos hilos
	//pthread_join(hiloConsola, NULL);
	//pthread_join(hiloServidor, NULL);

	//Para las conexiones, mas adelante falta agregar que si
	//estadoEstable == 0
	//No permita conexiones de Workers o YAMA


	//inicializarNodo(2);
	//inicializarNodo(1);
	//cantidadTotalBloquesLibres();

	createDirectory("root/some");
	//createDirectory("root/some/other");
	//createDirectory("root/ro");
	//createDirectory("root/some/carpeta"); //TODO con este rompe en el find

	//tablaDeDirectoriosEnArchivo();
	//createDirectory("some/dir")


	//almacenarArchivo("Nodo1.bin","","bin");
	//almacenarArchivo("Nodo10.txt","","txt");
	//importarArchivo("Nodo1.bin","");

	//copiaLocalAlYamafs("/home/utnso/Nuevo.txt", "root");


	return EXIT_SUCCESS;
}
