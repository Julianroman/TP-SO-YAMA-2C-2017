#include "funcionesFS.h"

t_log *log; // Log file

int32_t tamanioBloques = 1048576; // tamaño bloques 1MB


t_list *listaDeNodos; // Lista de nodos
t_directory *tablaDeDirectorios; // Tabla de directorios

static char *directorioRaiz = "/home/utnso/workspace/tp-2017-2c-Grupo-1---K3525/FileSystem/root/";
static char *pathArchivos = "metadata/archivos/";



t_nodo *nodo_create(int32_t nroNodo, t_bitarray* bitmap, int32_t socket, int32_t cantidadBloques) {
	t_nodo *new = malloc(sizeof(t_nodo));
    new->nroNodo = nroNodo;
    new->bitmap = bitmap;
    new->socket = socket;
    new->cantidadBloques = cantidadBloques;
    return new;
}

void nodo_destroy(t_nodo *self) {
    free(self->bitmap);
    free(self);
}

void directory_destroy(t_directory *self) {
    free(self->nombre);

    free(self);
}

void servidorFs(int puerto){

	fd_set master, masterAux; // conjunto maestro de descriptores de fichero y uno auxiliar para el select()
	FD_ZERO(&master);    // borra los conjuntos maestro y temporal
	FD_ZERO(&masterAux);
	t_dictionary* diccionario= dictionary_create();

	// obtener socket a la escucha
	int servidor = socket(AF_INET, SOCK_STREAM, 0);
	if (servidor == -1) {
		perror("Falló el socket");
		exit(1);
	}
	// obviar el mensaje "address already in use" (la dirección ya se está usando)
	int activado = 1;
	int socapete = setsockopt(servidor, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));
	if (socapete == -1) {
		perror("Falló el setsockopt");
		exit(1);
	}

	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = INADDR_ANY;
	direccionServidor.sin_port = htons(puerto);
	memset(&(direccionServidor.sin_zero), '\0', 8);

	if (bind(servidor, (void*) &direccionServidor, sizeof(direccionServidor)) != 0) {
		perror("Falló el bind");
		exit(1);
	}
	// escuchar
	printf("Esperando conexiones...\n");
	if (listen(servidor, 100) == -1) {
		perror("Falló el listen");
		exit(1);
	}

	// añadir servidor al conjunto maestro
	FD_SET(servidor, &master);
	// seguir la pista del descriptor de fichero mayor
	int fdmax = servidor; // por ahora es éste
	struct sockaddr_in direccionCliente; // dirección del cliente
	char* buffer = malloc(1000);
	char* proceso;
	// bucle principal
	while(1) {
		masterAux = master; // cópialo
		if (select(fdmax+1, &masterAux, NULL, NULL, NULL) == -1) {
			perror("Falló el select");
			exit(1);
		}
		// explorar conexiones existentes en busca de datos que leer
		int i;
		for(i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &masterAux)) { // ¡¡tenemos datos!!
				if (i == servidor) {
					// gestionar nuevas conexiones
					int addrlen = sizeof(direccionCliente);
					int cliente = accept(servidor, (void*) &direccionCliente, &addrlen);
					if (cliente  == -1) {
						perror("Falló el accept");
						exit(1);
					} else {
						FD_SET(cliente, &master); // añadir al conjunto maestro
						if (cliente > fdmax) {    // actualizar el máximo
							fdmax = cliente;
						}
						//char* nombreCliente = inet_ntoa(direccionCliente.sin_addr);
						proceso = malloc(16);
						snprintf(proceso, 16, "%d", 0);
						if(dictionary_get(diccionario, proceso) == 0){
							dictionary_put(diccionario, proceso, cliente);
						}
						free(proceso);
						//vector[cliente]= "0";
						char* mensaje = "Bienvenido!!";
						send(cliente, mensaje, strlen(mensaje), 0);
					}
				} else {
					// gestionar datos de un cliente
					proceso = malloc(16);
					snprintf(proceso, 16, "%d", i);
					payload_PRESENTACION_DATANODE * payloadCliente = dictionary_get(diccionario, proceso);
					//if(vector[i] == "0"){
					if(dictionary_get(diccionario, proceso) == 0){
						HEADER_T* cabecera;
						void* data;
						data = receive(i,&cabecera);
						payload_PRESENTACION_DATANODE * payload = data;
						//payload tiene toda la info
						dictionary_put(diccionario, proceso, payload);
						payloadCliente = dictionary_get(diccionario, proceso);
						printf("Recibí una conexión de DataNode %d!!\n", payloadCliente->id_dataNode);

						inicializarNodo(payloadCliente->id_dataNode, i, payloadCliente->cantidad_bloques);

						free(proceso);
						}else{
							int bytesRecibidos = recv(i, buffer, 1000, 0);
							if (bytesRecibidos <= 0) {
								// error o conexión cerrada por el cliente
								printf("El DataNode %d se desconectó\n", payloadCliente->id_dataNode);
								dictionary_remove(diccionario, proceso);
								close(i); // bye!
								FD_CLR(i, &master); // eliminar del conjunto maestro
							} else {
								buffer[bytesRecibidos] = '\0';
								printf("El DataNode %d dice: %s\n", payloadCliente->id_dataNode, buffer);
							}
							free(proceso);
						}
					} // Esto es ¡TAN FEO!
				}
			}
		}
		free(buffer);
}

// Trae 2 bloques para original y copia
// La idea es lograr que la tabla de nodos este lo mas balanceada posible
// O sea, traer siempre los bloques con mayor cantidad de bloques libres
static t_bloque_libre *traerBloquesLibres() {

	t_bloque_libre *retVal = (t_bloque_libre *)malloc(sizeof(t_bloque_libre)*2);

	// El nodo con mayor cantidad de bloques libres
	int nMayor = -1;
	// La mayor cantidad de bloques libres
	int nBloques = -1;

	int i;
	for (i = 0; i < list_size(listaDeNodos); i ++){
		t_nodo *unNodo;
		unNodo = list_get(listaDeNodos, i);
		int n = bloquesLibresEnNodo(unNodo);
		if ( n > nBloques ){
			nMayor = i;
			nBloques = n;
		}
	}
	// TODO: chequear que haya espacio

	retVal[0].nodo = list_get(listaDeNodos,nMayor);
	retVal[0].bloque = proximoBloqueLibre(retVal[0].nodo);
	//Modifico el bitmap del nodo
	escribirBloqueLibre(retVal[0].nodo, retVal[0].bloque);

	// Busco otro nodo con la mayor cantidad de bloques libres pero que sea distinto al anterior
	// El nodo con mayor cantidad de bloques libres (que no sea igual al primero)
	int nSegundoMayor = -1;
	// La mayor cantidad de bloques libres
	int nSegundoBloques = -1;

	for (i = 0; i < list_size(listaDeNodos); i ++){
		t_nodo *unNodo;
		unNodo = list_get(listaDeNodos, i);
		if( i != nMayor){
			int n = bloquesLibresEnNodo(unNodo);

			if ( n > nSegundoBloques ){
				nSegundoMayor = i;
				nSegundoBloques = n;
			}
		}
	}
	retVal[1].nodo = list_get(listaDeNodos,nSegundoMayor);
	retVal[1].bloque = proximoBloqueLibre(retVal[1].nodo);
	//Modifico el bitmap del nodo
	// TODO: verificar que no sea -1
	escribirBloqueLibre(retVal[1].nodo, retVal[1].bloque);

	return retVal;
}

int enviarADataNode(t_pagina *unaPagina, t_config *fileExport, int nroBloque){
	char *nombreBloque;
	char *almacenamientoBloque;

	// Busco 2 nodos con bloques libres
	t_bloque_libre *bloquesLibres = traerBloquesLibres();

	// Envio el original
	nombreBloque = string_new();
	almacenamientoBloque = string_new();

	string_append(&nombreBloque, "BLOQUE");
	string_append(&nombreBloque, string_itoa(nroBloque));
	string_append(&nombreBloque, "COPIA");
	string_append(&nombreBloque, "0");
	printf("Enviado a nodo: %i -- bloque %i \n", bloquesLibres[0].nodo->nroNodo, bloquesLibres[0].bloque);
	string_append(&almacenamientoBloque, "[Nodo");
	string_append(&almacenamientoBloque, string_itoa(bloquesLibres[0].nodo->nroNodo));
	string_append(&almacenamientoBloque, ", ");
	string_append(&almacenamientoBloque, string_itoa(bloquesLibres[0].bloque));
	string_append(&almacenamientoBloque, "]");
	config_set_value(fileExport, nombreBloque, almacenamientoBloque);

	//Envio el original al primer nodo
	//send_BLOQUE(bloquesLibres[0].nodo->socket, unaPagina->tamanio, unaPagina->contenido, bloquesLibres[0].nodo->nroNodo);


	// Envio la copia
	nombreBloque = string_new();
	almacenamientoBloque = string_new();

	string_append(&nombreBloque, "BLOQUE");
	string_append(&nombreBloque, string_itoa(nroBloque));
	string_append(&nombreBloque, "COPIA");
	string_append(&nombreBloque, "1");
	printf("Enviado a nodo: %i -- bloque %i \n", bloquesLibres[1].nodo->nroNodo, bloquesLibres[1].bloque);
	string_append(&almacenamientoBloque, "[Nodo");
	string_append(&almacenamientoBloque, string_itoa(bloquesLibres[1].nodo->nroNodo));
	string_append(&almacenamientoBloque, ", ");
	string_append(&almacenamientoBloque, string_itoa(bloquesLibres[1].bloque));
	string_append(&almacenamientoBloque, "]");
	config_set_value(fileExport, nombreBloque, almacenamientoBloque);

	//Envio la copia al segundo nodo
	//send_BLOQUE(bloquesLibres[1].nodo->socket, unaPagina->tamanio, unaPagina->contenido, bloquesLibres[1].nodo->nroNodo);

	// Libero la estructura
	free(bloquesLibres);
	free(nombreBloque);
	free(almacenamientoBloque);


	return 0;

}


static t_list *cortar_modo_texto(FILE *in){
	t_list *retVal = list_create();

		if (!(in)){
			log_error(log, "El archivo no existe o no se pudo abrir");
		}
		else{
			//validar si el destino es valido
			int size_bytes;
			fseek(in,0,SEEK_END);
			size_bytes = ftell(in);
			rewind(in);

			int cant_bloques = (size_bytes/tamanioBloques) + (size_bytes % tamanioBloques != 0);
			int tam = 0;
			char* map;
			if((map = mmap(NULL, size_bytes, PROT_READ, MAP_SHARED, fileno(in),0)) == MAP_FAILED){
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
					//enviarADataNode(text, bloq, tam, size_concat); // TODO Enviar a dataNode
					t_pagina *nodo = malloc(sizeof(t_pagina));
					nodo->tamanio = size_concat;
					nodo->contenido = malloc(size_concat);
					memcpy(nodo->contenido,text,size_concat);
					list_add(retVal, nodo);


					bloq ++;
					text = string_new();
				}
				i++;
			}
			if(!string_is_empty(text)){
				size_concat = strlen(text) * sizeof(char);
				//printf("%s\n",text);
				//enviarADataNode(text, bloq, tam, size_concat); // TODO Enviar a dataNode
				t_pagina *nodo = malloc(sizeof(t_pagina));
				nodo->tamanio = size_concat;
				nodo->contenido = malloc(size_concat);
				memcpy(nodo->contenido,text,size_concat);
				list_add(retVal, nodo);

				bloq ++;
			}
			free(str1);
			free(map);
			free(text);
		}
		fclose(in);


	return retVal;
}

static t_list *cortar_modo_binario(FILE *in){
	t_list *retVal = list_create();
	int n;
	char *buffer = malloc(tamanioBloques);

	while ( (n=fread(buffer,1,tamanioBloques,in)) > 0 ) {
		t_pagina *nodo = malloc(sizeof(t_pagina));
		nodo->tamanio = n;
		nodo->contenido = malloc(n);
		memcpy(nodo->contenido,buffer,n);
		list_add(retVal, nodo);
	}
	free(buffer);
	return retVal;
}

int almacenarArchivo(char *location, char *pathDestino, char *name, char *tipo){
	// Concateno el path del origen con el nombre y el tipo
	char *pathOrigenCompleto = string_new();
	pathOrigenCompleto = string_duplicate(location);
	if(!string_ends_with(location, "/"))
		string_append(&pathOrigenCompleto, "/");

	string_append(&pathOrigenCompleto, name);
	string_append(&pathOrigenCompleto, ".");
	string_append(&pathOrigenCompleto, tipo);


	// Agarro el path destino y le concateno el nombre

	char *pathDestinoCompleto = string_new();
	pathDestinoCompleto = string_duplicate(pathDestino);
	if(!string_ends_with(pathDestino, "/"))
		string_append(&pathDestinoCompleto, "/");

	string_append(&pathDestinoCompleto, name);

	char **arrayDestino = string_split(pathDestinoCompleto, "/");
	int cant = 0;
	while(arrayDestino[cant] != NULL ){
		cant++;
	}

	// Busco el indice de la carpeta de destino
	int indice = findDirByname(arrayDestino[cant - 2]);
	// Concateno el path con el indice y el path de los archivos
	char *indicePath = string_new();
	string_append(&indicePath, pathArchivos);
	string_append(&indicePath, string_itoa(indice));
	// Creo el directorio de nombre:  numero de indice (Si no existe)
	createDirectory(indicePath);

	// Concateno el path con el nombre del archivo
	string_append(&indicePath, "/");
	string_append(&indicePath, name);

	// Abro o creo un archivo de configuracion para ir guardando donde esta cada bloque
	// Seria la tabla de archivos
	char *pathArchivoConfig = string_new();
	string_append(&pathArchivoConfig, directorioRaiz);
	string_append(&pathArchivoConfig, indicePath);

	FILE *archivo = fopen(pathArchivoConfig, "w");

	t_config *fileExport = config_create(pathArchivoConfig);
	//config_set_value(fileExport, "FILE", destino);
	if(strcmp(tipo, "txt") == 0)
		config_set_value(fileExport, "TIPO", "TEXTO");
	else
		config_set_value(fileExport, "TIPO", "BINARIO");

	// Creo una lista de paginas donde almaceno estructuras de tipo t_pagina
	// Refleja el archivo leido
	// En el caso de binario, todos los bloques miden lo mismo salvo el ultimo que puede medir menos
	// En el caso de texto, cada bloque mide 1MB o menos

	FILE *in;

	t_list *lista_de_paginas;

	if ( strcmp(tipo,"txt") == 0 ) {
		if ( (in = fopen(pathOrigenCompleto, "r") ) == NULL ) {
			printf("No se encontro el archivo");
			return 1;
		}

		lista_de_paginas = cortar_modo_texto(in);

	}
	else{
		if ( (in = fopen(pathOrigenCompleto, "rb") ) == NULL ) {
			printf("No se encontro el archivo");
			return 1;
		}
		lista_de_paginas = cortar_modo_binario(in);
	}

	// Tamanio del archivo
	int size_bytes;
	fseek(in,0,SEEK_END);
	size_bytes = ftell(in);
	rewind(in);
	config_set_value(fileExport, "TAMANIO", string_itoa(size_bytes));

	int i;
	// Itero entre las paginas de la lista y se las mando a dataNode
	for ( i=0; i<list_size(lista_de_paginas); i++){
		t_pagina *pagina = list_get(lista_de_paginas, i);
		char* bloqueNro = string_new();
		string_append(&bloqueNro, "BLOQUE");
		string_append(&bloqueNro, string_itoa(i));
		string_append(&bloqueNro, "BYTES");
		config_set_value(fileExport, bloqueNro, string_itoa(pagina->tamanio));
		enviarADataNode(pagina, fileExport, i);
		free(pagina->contenido);
		free(pagina);
		//free(bloqueNro);

	}


	config_save(fileExport);
	config_save_in_file(fileExport, indicePath);

	fclose(archivo);
	config_destroy(fileExport);
	free(pathOrigenCompleto);
	free(indicePath);
	free(arrayDestino);

	fclose(in);
	return 0;
}

char *leerArchivo(char *pathConNombre){
	//TODO
	int cantidadDeBloques;

	// Para leer la tabla de archivos
	// Separo el path del destino con las /

	char **arrayPath = string_split(pathConNombre, "/");
	int cant = 0;
	while(arrayPath[cant] != NULL ){
		cant++;
	}

	// Agarro el nombre sin la extension
	char *name = string_new();
	name = arrayPath[cant - 1];

	if(string_contains(name, ".")){
		name = string_substring_until(name, strlen(name) - 4);
	}

	// Busco el indice de la carpeta de destino
	int indice = findDirByname(arrayPath[cant - 2]);
	// Concateno el path con el indice y el path de los archivos
	char *indicePath = string_new();

	// Entro al directorio de nombre:  numero de indice (Si no existe)
	string_append(&indicePath, pathArchivos);
	string_append(&indicePath, string_itoa(indice));

	// Concateno el path con el nombre del archivo
	string_append(&indicePath, "/");
	string_append(&indicePath, name);

	// Abro el archivo de configuracion que tiene la tabla del archivo
	char *pathArchivoConfig = string_new();
	string_append(&pathArchivoConfig, directorioRaiz);
	string_append(&pathArchivoConfig, indicePath);

	t_config* archivo_configuracion = config_create(pathArchivoConfig);

	int tamanio;
	tamanio = config_get_int_value(archivo_configuracion, "TAMANIO");

	char *tipo = string_new();
	tipo = config_get_string_value(archivo_configuracion, "TIPO");

	if(string_equals_ignore_case(tipo, "BINARIO")){
		if ( tamanio % tamanioBloques != 0 )
			cantidadDeBloques = tamanio/tamanioBloques +1;
		else
			cantidadDeBloques = tamanio/tamanioBloques;

		char *propertyBloque;
		char **nodoYBloque = malloc(sizeof(char*)*2);
		int i;
		for( i=0; i < cantidadDeBloques; i++ ){
			//
			propertyBloque = string_new();
			string_append(&propertyBloque, "BLOQUE");
			string_append(&propertyBloque, string_itoa(i));
			string_append(&propertyBloque, "COPIA0");


			nodoYBloque = string_get_string_as_array(config_get_string_value(archivo_configuracion, propertyBloque));

			// Leo del original si se puede.. Si no deberia buscar la copia
			printf("Leido de %s -- bloque %s \n", nodoYBloque[0], nodoYBloque[1]);
			// TODO: pedir bloques a data node


		}
		//config_has_property(t_config*, char* key);
	}


	config_destroy(archivo_configuracion);

	return "";
}

void formatear(){
	/*if(existeUnEstadoAnterior){
		restaurarDesdeAhi
	}*/


	/*estadoEstable = 1;
	formateado = 1;*/
}

void almacenarBitmapEnArchivo(t_nodo *unNodo){
	char* name = string_itoa(unNodo->nroNodo);
	char *pathNewBitmap = malloc(1000);
	strcpy(pathNewBitmap, PATHBITMAP);
	string_append(&pathNewBitmap, "nodo");
	string_append(&pathNewBitmap, name);
	string_append(&pathNewBitmap, ".dat");

	FILE* archivoBitmap;
	archivoBitmap = fopen(pathNewBitmap,"wb");

	//TODO
	if (archivoBitmap != NULL) {
		fwrite(&(unNodo->bitmap->size),sizeof(size_t),1,archivoBitmap);
		fwrite(unNodo->bitmap->bitarray,sizeof(char),unNodo->bitmap->size,archivoBitmap);
		fwrite(&(unNodo->bitmap->mode), (sizeof(bit_numbering_t)), 1, archivoBitmap);
	}

	log_trace(log, "El bitmap fue almacenado en: %s", pathNewBitmap);

	fclose(archivoBitmap); // TODO: Rompe

	free(pathNewBitmap);
}

int bloquesLibresEnNodo(t_nodo* unNodo){
	int cantidad = 0;

	int j;
	for (j = 0; j < unNodo->cantidadBloques; j++) {
		bool a = bitarray_test_bit(unNodo->bitmap, j); //TODO rompe
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
		t_nodo *unNodo;// = malloc(sizeof(t_nodo));
		unNodo = list_get(listaDeNodos, i);
		cantidad += bloquesLibresEnNodo(unNodo);
		//nodo_destroy(unNodo);
	}
	log_trace(log,"Total bloques libres: %d", cantidad);
	return cantidad;
}


void escribirBloqueLibre(t_nodo* unNodo,int bloque){

	//ESCRIBO LO QUE TENGA QUE ESCRIBIR Y DESPUES CAMBIO EL BITMAP Y LO ALMACENO
	bitarray_set_bit(unNodo->bitmap, bloque);
	printf("bloque %i\n",bloque);
	almacenarBitmapEnArchivo(unNodo);
}
int proximoBloqueLibre(t_nodo* unNodo){
	int j;
	for (j = 0; j < unNodo->cantidadBloques; j++) {
		bool a = bitarray_test_bit(unNodo->bitmap, j);
		if(a == 0){
			log_trace(log, "El proximo bloque libre del nodo %d es el: %d",unNodo->nroNodo, j);
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
		printf("%i", a);
	}
	log_info(log,"\n");
}

// El menor entero ...
static int iCeil(int n) {
	if ( n% 8 != 0 )
		return n/8+1;
	else
		return n/8;
}

t_bitarray* initOrCreateBitmap(int nroNodo, int cantidadDeBloques){
	char* name = string_itoa(nroNodo);
	char *pathNewBitmap = malloc(1000);
	strcpy(pathNewBitmap, PATHBITMAP);
	string_append(&pathNewBitmap, "nodo");
	string_append(&pathNewBitmap, name);
	string_append(&pathNewBitmap, ".dat");

	t_bitarray *unBitarray;

	FILE* bitmap;
	if (!(bitmap = fopen(pathNewBitmap, "r"))){
		log_info(log, "El bitmap del nodo %i no existe. Se inicializara.", nroNodo);
		int j = 0;
		char data[cantidadDeBloques];
		for(j = 0; j < cantidadDeBloques; j++){
			data[j] = '\0';
		}


		unBitarray = bitarray_create_with_mode(data,iCeil(cantidadDeBloques), LSB_FIRST);
		char * puntero = malloc(sizeof(char)*cantidadDeBloques);
		memcpy(puntero, unBitarray->bitarray, sizeof(char)*cantidadDeBloques);
		unBitarray->bitarray = puntero;
		int i;
		for(i = 0; i < cantidadDeBloques; i++){
			bitarray_clean_bit(unBitarray, i);
		}
		bitmap = fopen(pathNewBitmap, "wb");
		if (bitmap != NULL) {
			fwrite(&(unBitarray->size),sizeof(size_t),1,bitmap);
			fwrite(unBitarray->bitarray,sizeof(char),unBitarray->size,bitmap);
			fwrite(&(unBitarray->mode), (sizeof(bit_numbering_t)), 1, bitmap);
			fclose(bitmap);
		}

	}
	else{
		unBitarray = malloc(sizeof(t_bitarray));
		bitmap= fopen(pathNewBitmap, "rb");
		if (bitmap != NULL) {
			// TODO
			fread(&(unBitarray->size),sizeof(size_t),1,bitmap);

			unBitarray->bitarray = (char*) malloc(unBitarray->size * sizeof(char));
			fread(unBitarray->bitarray,sizeof(char),unBitarray->size,bitmap);
			fread(&(unBitarray->mode), (sizeof(bit_numbering_t)), 1, bitmap);

			fclose(bitmap);
		}
		log_info(log, "El bitmap del nodo%i fue leido con exito.", nroNodo);
	}
	return unBitarray;
}

void inicializarNodo(int nroNodo, int socket, int cantidadBloques){
	t_bitarray* unBitarray = initOrCreateBitmap(nroNodo, cantidadBloques);

	t_nodo* nuevoNodo;
	nuevoNodo = nodo_create(nroNodo, unBitarray, socket, cantidadBloques);
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
				if(tablaDeDirectorios[j].indice == -1){
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
