#include "funcionesFS.h"

#define EXITO 1
#define FRACASO 0


int tamanioBloques = 1048576; // tamaño bloques 1MB

int formateado = 0;

t_list *listaDeNodos; // Lista de nodos
t_directory *tablaDeDirectorios; // Tabla de directorios
t_list *nodosParaEstable; // Lista de t_nodos_por_archivo para restaurar estado estable

static char *directorioRaiz = "root/";
static char *pathArchivos = "metadata/archivos/";
static char *pathTablaNodos = "root/metadata/nodos.bin";

t_config *fileNodos;

int socketYama;
char *contenidoLeido;

pthread_mutex_t mutexContenido;
sem_t binaryContenidoServidor;
sem_t binaryContenidoConsola;



t_nodo *nodo_create(int32_t nroNodo, t_bitarray* bitmap, int32_t socket, int32_t cantidadBloques, char *ip) {
	t_nodo *new = malloc(sizeof(t_nodo));
    new->nroNodo = nroNodo;
    new->bitmap = bitmap;
    new->socket = socket;
    new->cantidadBloques = cantidadBloques;
    new->ipNodo = ip;
    return new;
}

void nodo_destroy(t_nodo *self) {
    free(self->bitmap);
    free(self->ipNodo);
    free(self);
}

void directory_destroy(t_directory *self) {
    free(self->nombre);

    free(self);
}

void desconectarNodo(int id_dataNode){
	int i;
	for (i = 0; i < list_size(listaDeNodos); i ++){
		t_nodo *unNodo;
		unNodo = list_get(listaDeNodos, i);
		if ( unNodo->nroNodo == id_dataNode ){
			list_remove(listaDeNodos, i);
			nodo_destroy(unNodo);
		}
	}

	actualizarTablaDeNodos();
}

void servidorFs(int puerto){

	fd_set master, masterAux; // conjunto maestro de descriptores de fichero y uno auxiliar para el select()
	FD_ZERO(&master);    // borra los conjuntos maestro y temporal
	FD_ZERO(&masterAux);

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
						//vector[cliente]= "0";
						//char* mensaje = "Bienvenido a FS!!";
						//send(cliente, mensaje, strlen(mensaje), 0);
					}
				} else {
					// gestionar datos de un cliente
						HEADER_T cabecera;
						void* data;
						data = receive(i,&cabecera);

						if(cabecera == PRESENTACION_DATANODE){
							//if(esEstadoEstable() == 1){

								char* mensaje = "Bienvenido a FS!!";
								send(i, mensaje, strlen(mensaje), 0);

								payload_PRESENTACION_DATANODE * payload = data;
								//payload tiene toda la info
								printf("Recibí una conexión de DataNode %d!!\n", payload->id_dataNode);

								char *puntero = malloc(payload->tamanio_ipDatanode);
								memcpy(puntero, payload->ipDatanode,payload->tamanio_ipDatanode);


								inicializarNodo(payload->id_dataNode, i, payload->cantidad_bloques, puntero);

								// Informo si es estado estable
								esEstadoEstable();
								//Cuando se conecta un nodo deja de estar formateado
								formateado = 0;
							//}else{
								//TODO eliminar si no es nodo necesario para estado estable

							//}



						}else if(cabecera == PETICION_NODO){
							// Si el estado no esta estable rechazo al Yama
							if(esEstadoEstable() == 1){
								payload_PETICION_NODO *payload = data;
								socketYama = i;
								leerArchivo(payload->nombreArchivo);
							}else{
								send_RECHAZO_CONEXION(i);
								close(i); // bye!
								FD_CLR(i, &master); // eliminar del conjunto maestro
								log_trace(log, "Se desconecto el YAMA. Estado NO estable");

							}

						}else if (cabecera == FIN_COMUNICACION) {

							int indiceNodo = getIndiceNodoBySocket(i);

							if(indiceNodo != -1){
								t_nodo *nodoEncontrado = list_get(listaDeNodos, indiceNodo);

								log_trace(log, "Se desconecto el nodo %i", nodoEncontrado->nroNodo);
								desconectarNodo(nodoEncontrado->nroNodo);
							}else if(i == socketYama){
								log_trace(log, "Se desconecto el YAMA");
							}

							close(i); // bye!
							FD_CLR(i, &master); // eliminar del conjunto maestro
						} else if(cabecera == BLOQUE){
							sem_wait(&binaryContenidoServidor);
							payload_BLOQUE * payload = data;

							pthread_mutex_lock(&mutexContenido);
							string_append(&contenidoLeido, payload->contenido);
							pthread_mutex_unlock(&mutexContenido);
							printf("Leido OK bloque %i \n", payload->numero_bloque);

							sem_post(&binaryContenidoConsola);
						} else if(cabecera == ALMACENAR_ARCHIVO){
							payload_ALMACENAR_ARCHIVO * payload = data;
							almacenarArchivoWorker(payload->pathDestino, payload->nombre, payload->tipo, payload->contenido, payload->tamanio_contenido, i);
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
t_bloque_libre *traerBloquesLibres() {

	t_bloque_libre *retVal = (t_bloque_libre *)malloc(sizeof(t_bloque_libre)*2);

	// El nodo con mayor cantidad de bloques libres
	int nMayor = -1;
	// La mayor cantidad de bloques libres
	float nBloques = -1;

	int i;
	for (i = 0; i < list_size(listaDeNodos); i ++){
		t_nodo *unNodo;
		unNodo = list_get(listaDeNodos, i);
		//int n = bloquesLibresEnNodo(unNodo);
		float n = (float)bloquesLibresEnNodo(unNodo)/ (float)unNodo->cantidadBloques;
		if ( n > nBloques ){
			nMayor = i;
			nBloques = n;
		}
	}

	if(nMayor != -1){
		retVal[0].nodo = list_get(listaDeNodos,nMayor);
		retVal[0].bloque = proximoBloqueLibre(retVal[0].nodo);
		if(retVal[0].bloque != -1){
			// Modifico el bitmap del nodo
			escribirBloqueLibre(retVal[0].nodo, retVal[0].bloque);
		}else{
			log_error(log, "No se pudo escribir el bloque libre. Bloque %i", retVal[0].bloque);
		}
	}else{
		log_error(log, "No hay nodos con bloques libres");
	}

	// Busco otro nodo con la mayor cantidad de bloques libres pero que sea distinto al anterior
	// El nodo con mayor cantidad de bloques libres (que no sea igual al primero)
	int nSegundoMayor = -1;
	// La mayor cantidad de bloques libres
	float nSegundoBloques = -1;

	if(list_size(listaDeNodos) > 1){

		for (i = 0; i < list_size(listaDeNodos); i ++){
			t_nodo *unNodo;
			unNodo = list_get(listaDeNodos, i);
			if( i != nMayor){
				//int n = bloquesLibresEnNodo(unNodo);
				float n = (float)bloquesLibresEnNodo(unNodo)/ (float)unNodo->cantidadBloques;

				if ( n > nSegundoBloques ){
					nSegundoMayor = i;
					nSegundoBloques = n;
				}
			}
		}

		//Modifico el bitmap del nodo
		// TODO: PROBAR
		if(nSegundoMayor != -1){
			retVal[1].nodo = list_get(listaDeNodos,nSegundoMayor);
			retVal[1].bloque = proximoBloqueLibre(retVal[1].nodo);
			if(retVal[1].bloque != -1){
				escribirBloqueLibre(retVal[1].nodo, retVal[1].bloque);
			}else{
				log_error(log, "No se pudo escribir el bloque libre. Bloque %i", retVal[1].bloque);
			}
		}else{
			log_error(log, "No hay nodos con bloques libres");
		}

	}

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
	log_info(log, "Enviado a nodo: %i -- bloque %i \n", bloquesLibres[0].nodo->nroNodo, bloquesLibres[0].bloque);
	string_append(&almacenamientoBloque, "[Nodo");
	string_append(&almacenamientoBloque, string_itoa(bloquesLibres[0].nodo->nroNodo));
	string_append(&almacenamientoBloque, ", ");
	string_append(&almacenamientoBloque, string_itoa(bloquesLibres[0].bloque));
	string_append(&almacenamientoBloque, "]");
	config_set_value(fileExport, nombreBloque, almacenamientoBloque);

	//Envio el original al primer nodo
	send_BLOQUE(bloquesLibres[0].nodo->socket, unaPagina->tamanio, unaPagina->contenido, bloquesLibres[0].bloque);

	if(list_size(listaDeNodos) > 1){ //TODO validar que no sea -1
		// Envio la copia
		nombreBloque = string_new();
		almacenamientoBloque = string_new();

		string_append(&nombreBloque, "BLOQUE");
		string_append(&nombreBloque, string_itoa(nroBloque));
		string_append(&nombreBloque, "COPIA");
		string_append(&nombreBloque, "1");
		log_info(log, "Enviado a nodo: %i -- bloque %i \n", bloquesLibres[1].nodo->nroNodo, bloquesLibres[1].bloque);
		string_append(&almacenamientoBloque, "[Nodo");
		string_append(&almacenamientoBloque, string_itoa(bloquesLibres[1].nodo->nroNodo));
		string_append(&almacenamientoBloque, ", ");
		string_append(&almacenamientoBloque, string_itoa(bloquesLibres[1].bloque));
		string_append(&almacenamientoBloque, "]");
		config_set_value(fileExport, nombreBloque, almacenamientoBloque);

		//Envio la copia al segundo nodo
		send_BLOQUE(bloquesLibres[1].nodo->socket, unaPagina->tamanio, unaPagina->contenido, bloquesLibres[1].bloque);
	}


	// Libero la estructura
	free(bloquesLibres);
	free(nombreBloque);
	free(almacenamientoBloque);


	return 0;

}

static t_list *cortar_modo_texto(FILE *in){
	t_list *retVal = list_create();

	int n;
	char *buffer = malloc(tamanioBloques);
	int offset = 0;
	int j = 0;
	while ( (n=fread(buffer,1,tamanioBloques,in)) > 0 ) {
		//-----------------------

		j = n;
		if(n == tamanioBloques){

			while(buffer[j-1] != '\n'){
				j--;
				fseek(in,-1,SEEK_CUR);
			}

		}else{
			fseek(in,0,SEEK_END);
		}

		t_pagina *nodo = malloc(sizeof(t_pagina));
		nodo->tamanio = j;
		nodo->contenido = malloc(j);
		memcpy(nodo->contenido, string_substring(buffer, 0, j), j); //TODO
		list_add(retVal, nodo);
		offset += j;
	}
	free(buffer);

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
	if(list_size(listaDeNodos) == 0){
		log_error(log, "No hay nodos conectados");
	}else{

		if(esRutaYamaFS(pathDestino)){
			// Concateno el path del origen con el nombre y el tipo
			char *pathOrigenCompleto = string_new();
			pathOrigenCompleto = string_duplicate(location);
			if(!string_ends_with(location, "/"))
				string_append(&pathOrigenCompleto, "/");

			string_append(&pathOrigenCompleto, name);
			//string_append(&pathOrigenCompleto, ".");
			//string_append(&pathOrigenCompleto, tipo);


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
			// Itero entre las paginas de la lista y se las mando a dataNode (Si hay espacio)
			// TODO probar
			if( cantidadTotalBloquesLibres() >= list_size(lista_de_paginas)){
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
			}else{
				log_error(log, "No hay suficiente espacio para almacenar el archivo.");
			}




			config_save(fileExport);
			config_save_in_file(fileExport, indicePath);

			fclose(archivo);
			config_destroy(fileExport);
			free(pathOrigenCompleto);
			free(indicePath);
			free(arrayDestino);

			fclose(in);
		}

	}


	return 0;
}

int almacenarArchivoWorker(char* pathDestino, char *name, char *tipo, char *contenido, int tamanioContenido, int socketRecibido){
	if(list_size(listaDeNodos) == 0){
		log_error(log, "No hay nodos conectados");
	}else{
		if(esRutaYamaFS(pathDestino)){

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

			char *pathTemp = "root/temp.txt";
			FILE *archivoTemp;
			archivoTemp = fopen(pathTemp, "w");
			fwrite(contenido, tamanioContenido ,1, archivoTemp);
			fclose(archivoTemp);



			// Creo una lista de paginas donde almaceno estructuras de tipo t_pagina
			// Refleja el archivo leido
			// En el caso de binario, todos los bloques miden lo mismo salvo el ultimo que puede medir menos
			// En el caso de texto, cada bloque mide 1MB o menos

			FILE *in;

			t_list *lista_de_paginas;

			if ( strcmp(tipo,"txt") == 0 ) {
				if ( (in = fopen(pathTemp, "r") ) == NULL ) {
					printf("No se encontro el archivo");
					return 1;
				}

				lista_de_paginas = cortar_modo_texto(in);

			}
			else{
				if ( (in = fopen(pathTemp, "rb") ) == NULL ) {
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
			// Itero entre las paginas de la lista y se las mando a dataNode (Si hay espacio)
			// TODO probar
			if( cantidadTotalBloquesLibres() >= list_size(lista_de_paginas)){
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
			}else{
				log_error(log, "No hay suficiente espacio para almacenar el archivo.");
			}


			config_save(fileExport);
			config_save_in_file(fileExport, indicePath);

			fclose(archivo);
			config_destroy(fileExport);
			free(indicePath);
			free(arrayDestino);

			fclose(in);

			// Elimino el archivo temporal donde estaba guardando el contenido
			if(remove(pathTemp) == -1){
				//No se elimino
			}
			free(pathTemp);
			send_EXITO_OPERACION(socketRecibido);

		}


	}


	return 0;
}



void enviarAYama(int numNodo, int bloqueDelNodo, int bloqueDelArchivo, int copia, char *ipDatanode, int tamanioBloque){
	send_UBICACION_BLOQUE(socketYama, ipDatanode , 5042 , numNodo, bloqueDelNodo, bloqueDelArchivo, copia, tamanioBloque);
}

void leerArchivo(char *pathConNombre){
	// Para leer la tabla de archivos
	// Separo el path con las /

	char **arrayPath = string_split(pathConNombre, "/");
	int cant = 0;
	while(arrayPath[cant] != NULL ){
		cant++;
	}

	// Agarro el nombre sin la extension
	char *name = string_new();
	name = arrayPath[cant - 1];

	if(esRutaYamaFSConNombre(pathConNombre)){
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
		contenidoLeido = string_new();
		FILE *in;
		if ( (in = fopen(pathArchivoConfig, "r") ) == NULL ) {
			log_error(log, "No se encontro el archivo");
			contenidoLeido = "Error";
			send_FRACASO_OPERACION(socketYama);
		}else{
			t_config* archivo_configuracion = config_create(pathArchivoConfig);

			char **nodoYBloque = malloc(sizeof(char*)*2);
			char **nodoYBloqueCopia = malloc(sizeof(char*)*2);

			int ok = 1;
			int i = 0;
			while(ok == 1){
				if(config_has_property(archivo_configuracion ,string_from_format("BLOQUE%iCOPIA0", i))){
					nodoYBloque = string_get_string_as_array(config_get_string_value(archivo_configuracion, string_from_format("BLOQUE%iCOPIA0", i)));
					log_info(log, "Leido de %s -- bloque %s -- ORDEN %i -- Original \n", nodoYBloque[0], nodoYBloque[1], i);

					// Agarro el tamanio del bloque
					int tamanioBloque = config_get_int_value(archivo_configuracion, string_from_format("BLOQUE%iBYTES", i));

					// Se envia a YAMA
					int nroNodo = atoi(string_substring_from(nodoYBloque[0],4));
					enviarAYama(nroNodo, atoi(nodoYBloque[1]), i, 0, getIpNodoByName(nroNodo), tamanioBloque);
				}

				if(config_has_property(archivo_configuracion ,string_from_format("BLOQUE%iCOPIA1", i))){
					nodoYBloqueCopia = string_get_string_as_array(config_get_string_value(archivo_configuracion, string_from_format("BLOQUE%iCOPIA1", i)));
					log_info(log, "Leido de %s -- bloque %s -- ORDEN %i -- Copia \n", nodoYBloqueCopia[0], nodoYBloqueCopia[1], i);

					// Agarro el tamanio del bloque
					int tamanioBloque = config_get_int_value(archivo_configuracion, string_from_format("BLOQUE%iBYTES", i));

					// Se envia a YAMA
					int nroNodoCopia = atoi(string_substring_from(nodoYBloqueCopia[0],4));
					enviarAYama(nroNodoCopia, atoi(nodoYBloqueCopia[1]), i, 1, getIpNodoByName(nroNodoCopia), tamanioBloque);
				}

				if(!config_has_property(archivo_configuracion ,string_from_format("BLOQUE%iCOPIA0", i)) && !config_has_property(archivo_configuracion ,string_from_format("BLOQUE%iCOPIA1", i))){
					if(i == 0){
						log_error(log, "No se encontro el archivo en la ruta indicada");
					}

					ok = 0;
				}

				i++;
			}

			free(nodoYBloque);
			free(nodoYBloqueCopia);
			free(archivo_configuracion);
			free(arrayPath);
			fclose(in);
			send_FIN_LISTA(socketYama);
		}
	}

}

void getInfoArchivo(char *pathConNombre){

	if(esRutaYamaFSConNombre(pathConNombre)){
		// Para leer la tabla de archivos
		// Separo el path con las /

		char **arrayPath = string_split(pathConNombre, "/");
		int cant = 0;
		while(arrayPath[cant] != NULL ){
			cant++;
		}

		// Agarro el nombre sin la extension
		char *name = string_new();
		name = arrayPath[cant - 1];

		/*if(string_contains(name, ".")){
			name = string_substring_until(name, strlen(name) - 4);
		}*/

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

		FILE *in;
		if ( (in = fopen(pathArchivoConfig, "r") ) == NULL ) {
			log_error(log, "No se encontro el archivo");
		}else{
			t_config* archivo_configuracion = config_create(pathArchivoConfig);

			char **nodoYBloque = malloc(sizeof(char*)*2);
			char **nodoYBloqueCopia = malloc(sizeof(char*)*2);

			if(config_has_property(archivo_configuracion ,"TAMANIO")){
				log_info(log, "TAMANIO: %i", config_get_int_value(archivo_configuracion ,"TAMANIO"));
			}
			if(config_has_property(archivo_configuracion ,"TIPO")){
				log_info(log, "TIPO: %s", config_get_string_value(archivo_configuracion ,"TIPO"));
			}

			int ok = 1;
			int i = 0;
			while(ok == 1){

				if(config_has_property(archivo_configuracion ,string_from_format("BLOQUE%iBYTES", i))){
					log_info(log, "Bloque %i", i);
					log_info(log, "Tamanio bloque: %i", config_get_int_value(archivo_configuracion, string_from_format("BLOQUE%iBYTES", i)));
				}


				if(config_has_property(archivo_configuracion ,string_from_format("BLOQUE%iCOPIA0", i))){
					nodoYBloque = string_get_string_as_array(config_get_string_value(archivo_configuracion, string_from_format("BLOQUE%iCOPIA0", i)));
					log_info(log, "ORIGINAL: %s -- Bloque %s\n", nodoYBloque[0], nodoYBloque[1], i);
				}

				if(config_has_property(archivo_configuracion ,string_from_format("BLOQUE%iCOPIA1", i))){
					nodoYBloqueCopia = string_get_string_as_array(config_get_string_value(archivo_configuracion, string_from_format("BLOQUE%iCOPIA1", i)));
					log_info(log, "COPIA: %s -- Bloque %s\n", nodoYBloqueCopia[0], nodoYBloqueCopia[1], i);

				}

				if(!config_has_property(archivo_configuracion ,string_from_format("BLOQUE%iCOPIA0", i)) && !config_has_property(archivo_configuracion ,string_from_format("BLOQUE%iCOPIA1", i))){
					ok = 0;
				}

				i++;
			}

			free(nodoYBloque);
			free(nodoYBloqueCopia);
			free(archivo_configuracion);
			fclose(in);
		}
	}


}

char *getIpNodoByName(int name){
	char* ipReturn = string_new();

	int i;
	for (i = 0; i < list_size(listaDeNodos); i ++){
		t_nodo *unNodo;
		unNodo = list_get(listaDeNodos, i);
		if ( unNodo->nroNodo == name ){
			ipReturn = unNodo->ipNodo;
		}
	}
	return ipReturn;
}

int getIndiceNodoBySocket(int nroSocket){
	int indice = -1;

	int i;
	for (i = 0; i < list_size(listaDeNodos); i ++){
		t_nodo *unNodo;
		unNodo = list_get(listaDeNodos, i);
		if ( unNodo->socket == nroSocket ){
			indice = i;
		}
	}
	return indice;
}

int getSocketNodoByName(int nroNodo){
	int socket = -1;
	int i;
	for (i = 0; i < list_size(listaDeNodos); i ++){
		t_nodo *unNodo;
		unNodo = list_get(listaDeNodos, i);
		if ( unNodo->nroNodo == nroNodo ){
			socket = unNodo->socket;
		}
	}
	return socket;
}

/*char *leerContenidoArchivo(char *pathConNombre){
	int cantidadDeBloques;

	sem_init(&binaryContenidoConsola, 0, 0);
	sem_init(&binaryContenidoServidor, 0, 0);

	// Para leer la tabla de archivos
	// Separo el path con las /

	char **arrayPath = string_split(pathConNombre, "/");
	int cant = 0;
	while(arrayPath[cant] != NULL ){
		cant++;
	}

	// Agarro el nombre sin la extension
	char *name = string_new();
	name = arrayPath[cant - 1];

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
	contenidoLeido = string_new();
	FILE *in;
	if ( (in = fopen(pathArchivoConfig, "r") ) == NULL ) {
		log_error(log, "No se encontro el archivo");
		contenidoLeido = "Error";
	}else{
		t_config* archivo_configuracion = config_create(pathArchivoConfig);

		int tamanio;
		tamanio = config_get_int_value(archivo_configuracion, "TAMANIO");

		pthread_mutex_lock(&mutexContenido);

		//contenidoLeido = malloc(tamanio);


		pthread_mutex_unlock(&mutexContenido);

		char *tipo = string_new();
		tipo = config_get_string_value(archivo_configuracion, "TIPO");

		if(string_equals_ignore_case(tipo, "BINARIO")){
			if ( tamanio % tamanioBloques != 0 )
				cantidadDeBloques = tamanio/tamanioBloques +1;
			else
				cantidadDeBloques = tamanio/tamanioBloques;

			char **nodoYBloque = malloc(sizeof(char*)*2);
			char **nodoYBloqueCopia = malloc(sizeof(char*)*2);
			int i;
			for( i=0; i < cantidadDeBloques; i++ ){
				// Agarro el tamanio del bloque
				int tamanioBloque = config_get_int_value(archivo_configuracion, string_from_format("BLOQUE%iBYTES", i));

				nodoYBloque = string_get_string_as_array(config_get_string_value(archivo_configuracion, string_from_format("BLOQUE%iCOPIA0", i)));
				// Pido el original
				printf("Pedido a %s -- bloque %s -- ORDEN %i -- Original \n", string_substring_from(nodoYBloque[0],4) , nodoYBloque[1], i);

				int socketOriginal = getSocketNodoByName(atoi(string_substring_from(nodoYBloque[0],4)));
				if(socketOriginal != -1){
					send_PETICION_BLOQUE(socketOriginal,atoi(nodoYBloque[1]), tamanioBloque);
					sem_post(&binaryContenidoServidor);
					sem_wait(&binaryContenidoConsola);

				}
				else{

					nodoYBloqueCopia = string_get_string_as_array(config_get_string_value(archivo_configuracion, string_from_format("BLOQUE%iCOPIA1", i)));
					// Si no se pudo agarrar el original, pido la copia
					printf("Pedido a %s -- bloque %s -- ORDEN %i -- Copia \n", nodoYBloqueCopia[0], nodoYBloqueCopia[1], i);

					int socketCopia = getSocketNodoByName(atoi(string_substring_from(nodoYBloqueCopia[0],4)));
					if(socketCopia != -1){
						send_PETICION_BLOQUE(socketCopia,atoi(nodoYBloqueCopia[1]), tamanioBloque);
						sem_post(&binaryContenidoServidor);
						sem_wait(&binaryContenidoConsola);
					}

				}

			}
			free(nodoYBloque);
			free(nodoYBloqueCopia);



		}else{
			char **nodoYBloque = malloc(sizeof(char*)*2);
			char **nodoYBloqueCopia = malloc(sizeof(char*)*2);
			int ok = 0;
			int i = 0;
			while(ok == 0){
				int leidoOriginal = 0;
				if(config_has_property(archivo_configuracion ,string_from_format("BLOQUE%iCOPIA0", i))){
					nodoYBloque = string_get_string_as_array(config_get_string_value(archivo_configuracion, string_from_format("BLOQUE%iCOPIA0", i)));
					printf("Leido de %s -- bloque %s -- ORDEN %i -- Original \n", nodoYBloque[0], nodoYBloque[1], i);

					// Agarro el tamanio del bloque
					int tamanioBloque = config_get_int_value(archivo_configuracion, string_from_format("BLOQUE%iBYTES", i));

					int socketOriginal = getSocketNodoByName(atoi(string_substring_from(nodoYBloque[0],4)));
					if(socketOriginal != -1){
						send_PETICION_BLOQUE(socketOriginal,atoi(nodoYBloque[1]), tamanioBloque);
						sem_post(&binaryContenidoServidor);
						sem_wait(&binaryContenidoConsola);
					}else{
						leidoOriginal = 1;
					}
				}

				if(leidoOriginal == 1 && config_has_property(archivo_configuracion ,string_from_format("BLOQUE%iCOPIA1", i))){
					nodoYBloqueCopia = string_get_string_as_array(config_get_string_value(archivo_configuracion, string_from_format("BLOQUE%iCOPIA1", i)));
					printf("Leido de %s -- bloque %s -- ORDEN %i -- Copia \n", nodoYBloqueCopia[0], nodoYBloqueCopia[1], i);

					// Agarro el tamanio del bloque
					int tamanioBloque = config_get_int_value(archivo_configuracion, string_from_format("BLOQUE%iBYTES", i));

					int socketCopia = getSocketNodoByName(atoi(string_substring_from(nodoYBloqueCopia[0],4)));
					if(socketCopia != -1){
						send_PETICION_BLOQUE(socketCopia,atoi(nodoYBloqueCopia[1]), tamanioBloque);
						sem_post(&binaryContenidoServidor);
						sem_wait(&binaryContenidoConsola);
					}else{
						ok = 1;
					}
				}else{
					if(leidoOriginal == 1){
						ok = 1;
					}
				}

				i++;
			}
		}
		free(tipo);
		//config_destroy(archivo_configuracion);
		fclose(in);
	}
	return contenidoLeido;
}*/

char *leerContenidoArchivo(char *pathConNombre){

	if(esRutaYamaFSConNombre(pathConNombre)){
		sem_init(&binaryContenidoConsola, 0, 0);
		sem_init(&binaryContenidoServidor, 0, 0);

		// Para leer la tabla de archivos
		// Separo el path con las /

		char **arrayPath = string_split(pathConNombre, "/");
		int cant = 0;
		while(arrayPath[cant] != NULL ){
			cant++;
		}

		// Agarro el nombre sin la extension
		char *name = string_new();
		name = arrayPath[cant - 1];

		/*if(string_contains(name, ".")){
			name = string_substring_until(name, strlen(name) - 4);
		}*/

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
		contenidoLeido = string_new();
		FILE *in;
		if ( (in = fopen(pathArchivoConfig, "r") ) == NULL ) {
			log_error(log, "No se encontro el archivo");
			contenidoLeido = "Error";
		}else{
			t_config* archivo_configuracion = config_create(pathArchivoConfig);

			char **nodoYBloque = malloc(sizeof(char*)*2);
			char **nodoYBloqueCopia = malloc(sizeof(char*)*2);
			int socketOriginal = -1;
			int socketCopia = -1;
			int ok = 1;
			int i = 0;
			while(ok == 1){
				if(config_has_property(archivo_configuracion ,string_from_format("BLOQUE%iCOPIA0", i))){
					nodoYBloque = string_get_string_as_array(config_get_string_value(archivo_configuracion, string_from_format("BLOQUE%iCOPIA0", i)));
					// Agarro el tamanio del bloque
					int tamanioBloque = config_get_int_value(archivo_configuracion, string_from_format("BLOQUE%iBYTES", i));

					socketOriginal = getSocketNodoByName(atoi(string_substring_from(nodoYBloque[0],4)));
					if(socketOriginal != -1){
						// Pido el original
						log_info(log, "Pedido a %s -- bloque %s -- ORDEN %i -- Original \n", string_substring_from(nodoYBloque[0],4) , nodoYBloque[1], i);
						send_PETICION_BLOQUE(socketOriginal,atoi(nodoYBloque[1]), tamanioBloque);
						sem_post(&binaryContenidoServidor);
						sem_wait(&binaryContenidoConsola);

					}

				}

				if(config_has_property(archivo_configuracion ,string_from_format("BLOQUE%iCOPIA1", i)) && socketOriginal == -1){
					nodoYBloqueCopia = string_get_string_as_array(config_get_string_value(archivo_configuracion, string_from_format("BLOQUE%iCOPIA1", i)));

					// Agarro el tamanio del bloque
					int tamanioBloque = config_get_int_value(archivo_configuracion, string_from_format("BLOQUE%iBYTES", i));

					int socketCopia = getSocketNodoByName(atoi(string_substring_from(nodoYBloqueCopia[0],4)));
					if(socketCopia != -1){
						// Pido la copia
						log_info(log, "Pedido a %s -- bloque %s -- ORDEN %i -- Copia \n", string_substring_from(nodoYBloqueCopia[0],4) , nodoYBloqueCopia[1], i);
						send_PETICION_BLOQUE(socketCopia,atoi(nodoYBloqueCopia[1]), tamanioBloque);
						sem_post(&binaryContenidoServidor);
						sem_wait(&binaryContenidoConsola);

					}
				}


				if(!config_has_property(archivo_configuracion ,string_from_format("BLOQUE%iCOPIA0", i)) && !config_has_property(archivo_configuracion ,string_from_format("BLOQUE%iCOPIA1", i))){
					ok = 0;
				}

				if(ok != 0 && socketOriginal != -1 && socketCopia != -1){
					log_error(log, "Los sockets no estan disponibles");
					contenidoLeido = "Error";
					ok = 0;
				}

				i++;
			}

			free(nodoYBloque);
			free(nodoYBloqueCopia);
			free(archivo_configuracion);
			fclose(in);
		}
		return contenidoLeido;
	}else{
		return "Error";
	}



}

void agregarNodoAListaSiNoExiste(t_list *lista, char *nodo){
	int existe = 0;

	int i;
	for(i = 0; i < list_size(lista); i++){
		char *a = list_get(lista, i);
		if(string_equals_ignore_case(a,nodo)){
			existe = -1;
		}
	}

	if(existe != -1 || list_size(lista) == 0){
		list_add(lista, nodo);
	}
}

int esEstadoEstable(){
	// Se fija si todos los nodos estan conectados
	// Devuelve 0 si no estan todos
	// Devuelve 1 si esta estable

	if(formateado == 1){
		//Si recien esta formateado esta estable
		log_trace(log, "El sistema se encuentra recien formateado ==> ESTABLE");
		return 1;
	}else{
		if(cantidadTotalDeArchivos()==0){
			log_trace(log, "El sistema se encuentra ESTABLE");
			return 1;
		}

		int estable = 1;

		char *stringNodosConectados = string_new();
		stringNodosConectados = listaDeNodosAsArray();

		//char **nodosConectados = string_get_string_as_array(stringNodosConectados);
		if(!list_is_empty(nodosParaEstable)){
			int i;
			for(i=0; i < list_size(nodosParaEstable); i++){
				t_nodos_por_archivo *nodosNecesarios = list_get(nodosParaEstable, i);
				int todosParaOriginal = 0;
				int todosParaCopia = 0;

				int j;
				for(j=0; j < list_size(nodosNecesarios->nodosOriginal); j++){
					if(!string_contains(stringNodosConectados, list_get(nodosNecesarios->nodosOriginal, j))){
						todosParaOriginal = -1;
					}
				}

				int k;
				for(k=0; k < list_size(nodosNecesarios->nodosCopia); k++){
					if(!string_contains(stringNodosConectados, list_get(nodosNecesarios->nodosCopia, k))){
						todosParaCopia = -1;
					}
				}

				if(todosParaOriginal == -1 && todosParaCopia == -1){
					estable = 0;
				}

				if((list_is_empty(nodosNecesarios->nodosOriginal) && todosParaCopia == -1) || (list_is_empty(nodosNecesarios->nodosCopia) && todosParaOriginal == -1)){
					estable = 0;
				}
			}
		}


		if(estable == 1)
			log_trace(log, "El sistema se encuentra en estado ESTABLE");
		else
			log_trace(log, "El sistema se encuentra en estado NO ESTABLE");

		return estable;
	}


}

int cantidadTotalDeArchivos(){
	int cantidad = 0;
	DIR *d;
	struct dirent *dir;
	// Abro el directorio que contiene todos los directorios de los archivos
	d = opendir(string_from_format("%s%s", directorioRaiz, pathArchivos));
	if (d){
		while ((dir = readdir(d)) != NULL)
		{
			// Por cada directorio que contenga archivos
			if(!string_equals_ignore_case(dir->d_name, ".") && !string_equals_ignore_case(dir->d_name, "..")){
				//Es un directorio que contiene archivos

				DIR *arch;
				struct dirent *archivos;
				arch = opendir(string_from_format("%s%s%s", directorioRaiz, pathArchivos, dir->d_name));
				if (arch){
					while ((archivos = readdir(arch)) != NULL){
						// Por cada archivo del directorio lo sumo

						cantidad ++;

					}
					closedir(arch);
				}

			}

		}

		closedir(d);
	}
	return cantidad;
}

void nodosARestaurar(){

	nodosParaEstable = list_create();

	DIR *d;
	struct dirent *dir;
	// Abro el directorio que contiene todos los directorios de los archivos
	d = opendir(string_from_format("%s%s", directorioRaiz, pathArchivos));
	if (d){
		while ((dir = readdir(d)) != NULL)
		{
			// Por cada directorio que contenga archivos
			if(!string_equals_ignore_case(dir->d_name, ".") && !string_equals_ignore_case(dir->d_name, "..")){
				//Es un directorio que contiene archivos

				DIR *arch;
				struct dirent *archivos;
				arch = opendir(string_from_format("%s%s%s", directorioRaiz, pathArchivos, dir->d_name));
				if (arch){
					while ((archivos = readdir(arch)) != NULL){
						// Por cada archivo del directorio tengo que agarrar los nodos necesarios para la copia 0 y la 1

						t_nodos_por_archivo *nodosNecesarios = malloc(sizeof(t_nodos_por_archivo));
						t_list *nodosParaOriginal = list_create();
						t_list *nodosParaCopia = list_create();

						if(!string_equals_ignore_case(archivos->d_name, ".") && !string_equals_ignore_case(archivos->d_name, "..")){
							printf("%s\n", archivos->d_name);

							t_config* currFile = config_create(string_from_format("%s%s%s/%s", directorioRaiz, pathArchivos, dir->d_name, archivos->d_name));

							int ok = 1;
							int i = 0;
							while(ok == 1){
								if(config_has_property(currFile ,string_from_format("BLOQUE%iCOPIA0", i))){
									char **nodoYBloque = malloc(sizeof(char*)*2);
									nodoYBloque = string_get_string_as_array(config_get_string_value(currFile, string_from_format("BLOQUE%iCOPIA0", i)));

									printf("Necesito nodo %i para la copia 0 \n", atoi(string_substring_from(nodoYBloque[0],4)));

									agregarNodoAListaSiNoExiste(nodosParaOriginal, string_duplicate(nodoYBloque[0]));


									free(nodoYBloque);

								}

								if(config_has_property(currFile ,string_from_format("BLOQUE%iCOPIA1", i))){
									char **nodoYBloque = malloc(sizeof(char*)*2);
									nodoYBloque = string_get_string_as_array(config_get_string_value(currFile, string_from_format("BLOQUE%iCOPIA1", i)));

									printf("Necesito nodo %i para la copia 1 \n", atoi(string_substring_from(nodoYBloque[0],4)));

									agregarNodoAListaSiNoExiste(nodosParaCopia, string_duplicate(nodoYBloque[0]));

									free(nodoYBloque);
								}

								if(!config_has_property(currFile ,string_from_format("BLOQUE%iCOPIA0", i)) && !config_has_property(currFile ,string_from_format("BLOQUE%iCOPIA1", i))){
									ok = 0;
								}

								i++;
							}

							nodosNecesarios->nodosOriginal = nodosParaOriginal;
							nodosNecesarios->nodosCopia = nodosParaCopia;

							list_add(nodosParaEstable, nodosNecesarios);
							config_destroy(currFile);


						}
					}
					closedir(arch);
				}

			}

		}

		closedir(d);
	}
}

int existeEstadoAnterior(){
	struct stat st = {0};
	if (stat(PATHDIRECTORIOS, &st) == -1) { //Si no existe el path
		log_info(log, "No se encontro el archivo de directorios. No hay un estado anterior para restaurar.");
		return 1;
	}
	else if(stat(pathTablaNodos, &st) == -1){
		log_info(log, "No se encontro la tabla de nodos. No hay un estado anterior para restaurar.");
		return 1;
	}
	else{
		return 0;
	}

}

void initOrRestoreFS(){
	if(existeEstadoAnterior() == 1){
		formatear();
	}else{
		initFS();
		nodosARestaurar();
		log_info(log, "Se encontro un estado anterior. Esperando nodos...");
	}
}

void initFS(){

	//Si se borró el /root lo inicializo
	struct stat st = {0};
	if (stat(directorioRaiz, &st) == -1) { //Si no existe el path, lo creo
		if(mkdir(directorioRaiz, 0700) == 0){
			log_info(log,"Se creo el directorio root");
		}
	}


	initTablaDeDirectorios();
	createDirectory("metadata");
	createDirectory("metadata/bitmaps");
	createDirectory("metadata/archivos");

	initTablaDeNodos();
}

void formatear(){
	system(string_from_format("rm -r %s/*", directorioRaiz));

	// la tabla de directorios esta en el directorio raiz asi que ya se borra antes
	/*if(remove(PATHDIRECTORIOS) == -1){
		//No se elimino
		log_error(log,"No se pudo eliminar la tabla de directorios");
	}*/

	//config_destroy(fileNodos);

	initFS();

	formateado = 1;
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

	if (archivoBitmap != NULL) {
		fwrite(&(unNodo->bitmap->size),sizeof(size_t),1,archivoBitmap);
		fwrite(unNodo->bitmap->bitarray,sizeof(char),unNodo->bitmap->size,archivoBitmap);
		fwrite(&(unNodo->bitmap->mode), (sizeof(bit_numbering_t)), 1, archivoBitmap);
	}

	log_trace(log, "El bitmap fue almacenado en: %s", pathNewBitmap);

	fclose(archivoBitmap);

	free(pathNewBitmap);
}

int bloquesLibresEnNodo(t_nodo* unNodo){
	int cantidad = 0;

	int j;
	for (j = 0; j < unNodo->cantidadBloques; j++) {
		bool a = bitarray_test_bit(unNodo->bitmap, j);
		if(a == 0){
			cantidad ++;
		}

	}

	return cantidad;
}

int cantidadTotalBloques(){
	int cantidad;
	cantidad = 0;
	//PARA CADA ELEMENTO DE LA LISTA
	int i;
	for (i = 0; i < list_size(listaDeNodos); i ++){
		t_nodo *unNodo;
		unNodo = list_get(listaDeNodos, i);
		cantidad += unNodo->cantidadBloques;
	}
	log_trace(log,"Total bloques: %d", cantidad);
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

	actualizarNodoEnTabla(unNodo);
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

char* listaDeNodosAsArray(){
	char *textArray = string_new();
	string_append(&textArray, "[");
	int i = 0;
	for(i=0; i < list_size(listaDeNodos); i++){
		if(i>0){
			string_append(&textArray, ", ");
		}
		t_nodo *unNodo;
		unNodo = list_get(listaDeNodos, i);
		string_append(&textArray, string_from_format("Nodo%d", unNodo->nroNodo));
	}
	string_append(&textArray, "]");

	return textArray;
}

void actualizarTablaDeNodos(){
	if(list_size(listaDeNodos) == 0){
		config_destroy(fileNodos);
		fileNodos = config_create(pathTablaNodos);
	}else{
		int i = 0;
		for(i=0; i < list_size(listaDeNodos); i++){
			t_nodo *unNodo;
			unNodo = list_get(listaDeNodos, i);

			config_set_value(fileNodos, string_from_format("Nodo%dTotal", unNodo->nroNodo), string_itoa(unNodo->cantidadBloques));

			int cantLibres = bloquesLibresEnNodo(unNodo);
			config_set_value(fileNodos, string_from_format("Nodo%dLibres", unNodo->nroNodo), string_itoa(cantLibres));

			config_set_value(fileNodos, "LIBRE", string_itoa(cantidadTotalBloquesLibres()));

			config_set_value(fileNodos, "NODOS", listaDeNodosAsArray());

			config_set_value(fileNodos, "TAMANIO", string_itoa(cantidadTotalBloques()));

		}
	}

	config_save_in_file(fileNodos, pathTablaNodos);

}

void actualizarNodoEnTabla(t_nodo *unNodo){
	config_set_value(fileNodos, string_from_format("Nodo%dTotal", unNodo->nroNodo), string_itoa(unNodo->cantidadBloques));

	int cantLibres = bloquesLibresEnNodo(unNodo);
	config_set_value(fileNodos, string_from_format("Nodo%dLibres", unNodo->nroNodo), string_itoa(cantLibres));

	config_set_value(fileNodos, "LIBRE", string_itoa(cantidadTotalBloquesLibres()));

	config_save(fileNodos);
}

void agregarNodoATabla(t_nodo *unNodo){
	config_set_value(fileNodos, string_from_format("Nodo%dTotal", unNodo->nroNodo), string_itoa(unNodo->cantidadBloques));

	int cantLibres = bloquesLibresEnNodo(unNodo);
	config_set_value(fileNodos, string_from_format("Nodo%dLibres", unNodo->nroNodo), string_itoa(cantLibres));

	config_set_value(fileNodos, "LIBRE", string_itoa(cantidadTotalBloquesLibres()));

	config_set_value(fileNodos, "NODOS", listaDeNodosAsArray());

	config_set_value(fileNodos, "TAMANIO", string_itoa(cantidadTotalBloques()));

	config_save(fileNodos);

}

void inicializarNodo(int nroNodo, int socket, int cantidadBloques, char *ipNodo){
	t_bitarray* unBitarray = initOrCreateBitmap(nroNodo, cantidadBloques);

	t_nodo* nuevoNodo;
	nuevoNodo = nodo_create(nroNodo, unBitarray, socket, cantidadBloques, ipNodo);
	list_add(listaDeNodos, nuevoNodo);

	agregarNodoATabla(nuevoNodo);


}

void initTablaDeNodos(){
	listaDeNodos = list_create();

	FILE *archivoNodos;
	if (!(archivoNodos = fopen(pathTablaNodos, "r"))){
		archivoNodos = fopen(pathTablaNodos, "w");
	}

	fileNodos = config_create(pathTablaNodos);
	if(!config_has_property(fileNodos, "TAMANIO")){
		config_set_value(fileNodos, "TAMANIO", "0");
	}
	if(!config_has_property(fileNodos, "LIBRE")){
		config_set_value(fileNodos, "LIBRE", "0");
	}
	if(!config_has_property(fileNodos, "NODOS")){
		config_set_value(fileNodos, "NODOS", "[]");
	}

	fclose(archivoNodos);
	config_save_in_file(fileNodos, pathTablaNodos);
}

void printLs(char* path){
	char **padres = string_split(path, "/");
	int cant = 0;
	while(padres[cant] != NULL ){
		cant++;
	}

	int indice = findDirByname(padres[cant - 1]);
	if(indice == -1){
		log_error(log, "No se encontro el directorio");
	}
	else{
		int i;
		for (i = 0; i < TOTALDIRECTORIOS; i++) {
			if(tablaDeDirectorios[i].padre == indice){
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

// Para fijarse si la ruta es una ruta valida dentro del YamaFS
// Ruta sin nombre del archivo
int esRutaYamaFS(char* path){
	char **ruta = string_split(path, "/");
	//int cant;
	//cant = strlen(padres) / sizeof(char*); //Length de padres
	int cant = 0;
	while(ruta[cant] != NULL ){
		if(findDirByname(ruta[cant]) == -1){
			free(ruta);
			log_error(log, "La ruta indicada no es una ruta valida en YamaFs");
			return FRACASO;
		}
		cant++;
	}

	free(ruta);
	return EXITO;

}

int esRutaYamaFSConNombre(char *pathConNombre){

	char **arrayPath = string_split(pathConNombre, "/");
	int cant = 0;
	while(arrayPath[cant] != NULL ){
		cant++;
	}

	char *rutaSinNombre = string_new();
	string_append(&rutaSinNombre, arrayPath[0]);
	int r;
	for(r=1; r < cant-1; r++){
		string_append(&rutaSinNombre, "/");
		string_append(&rutaSinNombre, arrayPath[r]);
	}

	free(arrayPath);

	return esRutaYamaFS(rutaSinNombre);
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
				char **padres = string_split(path, "/");
				//int cant;
				//cant = strlen(padres) / sizeof(char*); //Length de padres
				int cant = 0;
				while(padres[cant] != NULL ){
					cant++;
				}


				if(findDirByname(padres[cant-1]) == -1){ //Si es -1 no existe
					// TODO deberia borrar desde aca
					struct stat st = {0};

					if (stat(pathConcat, &st) == -1) { //Si no existe el path, lo creo
						if(mkdir(pathConcat, 0700) == 0){
					// hasta aca

							if(cant == 1){
								tablaDeDirectorios[indiceDisponible].indice = indiceDisponible;
								strcpy(tablaDeDirectorios[indiceDisponible].nombre, padres[0]);
								tablaDeDirectorios[indiceDisponible].padre = 0;
							}
							else{
								int father;
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
						// TODO deberia borrar desde aca
						}else{
							log_error(log, "Error al crear directorio");
						}
					}
					else{
						log_error(log, "El directorio ya existe o no se pudo crear");
					}// hasta aca

					free(padres);
				}else{
					log_error(log, "El directorio que intenta crear ya existe");
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
			int cant = 0;
			while(padres[cant] != NULL ){
				cant++;
			}

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
