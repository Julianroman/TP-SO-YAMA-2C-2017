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


t_log* log;
int32_t miPuerto = 5040;
int32_t cantBloques = 20;
int32_t tamanioBloques = 1048576; //1MB
int32_t bitmap[20];

char* pathBitmap = "metadata/bitmaps/";//Esto deberia estar en la carpeta metadata/bitmaps

int32_t estadoEstable = 0;
int32_t formateado = 0;

t_list* listaDeNodos;
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
		bool a = bitarray_test_bit(unNodo->bitmap, j); //TODO Rompe acá
		if(a == 0){
			cantidad ++;
		}
	}
	return cantidad;
}

int cantidadTotalBloquesLibres(){
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
	for (j = 0; j < 24; j++) {
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
	for (j = 0; j < 24; j++) {
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
	bitarray_destroy(unBitarray);

}

int main(int arg, char** argv) {
	log = log_create("fileSystem.log", "FileSystem", true, LOG_LEVEL_TRACE);
	log_trace(log, "Comienza el proceso FileSystem");

	listaDeNodos = list_create();

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


	inicializarNodo(2);
	inicializarNodo(1);
	cantidadTotalBloquesLibres();
	//almacenarArchivo("Nodo1.bin","","bin");
	//almacenarArchivo("Nodo10.txt","","txt");
	//importarArchivo("Nodo1.bin","");
	return EXIT_SUCCESS;
}
