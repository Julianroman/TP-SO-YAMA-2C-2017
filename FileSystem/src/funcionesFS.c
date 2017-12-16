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
char *lecturaPathMD5 = "LECTURAFILEMD5.txt";


t_config *fileNodos;

int socketYama;

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
	log_trace(log, "Esperando conexiones...\n");
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
								log_trace(log,"Recibí una conexión de DataNode %d!! \n", payload->id_dataNode);
								//printf("Recibí una conexión de DataNode %d!!\n", payload->id_dataNode);

								char *puntero = malloc(payload->tamanio_ipDatanode);
								memcpy(puntero, payload->ipDatanode,payload->tamanio_ipDatanode);
								free(payload->ipDatanode);

								inicializarNodo(payload->id_dataNode, i, payload->cantidad_bloques, puntero);

								// Informo si es estado estable
								esEstadoEstable();
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
								log_trace(log, "Se desconecto el YAMA. Estado NO estable \n");

							}

						}else if (cabecera == FIN_COMUNICACION) {

							int indiceNodo = getIndiceNodoBySocket(i);

							if(indiceNodo != -1){
								t_nodo *nodoEncontrado = list_get(listaDeNodos, indiceNodo);

								log_trace(log, "Se desconecto el nodo %i \n", nodoEncontrado->nroNodo);
								desconectarNodo(nodoEncontrado->nroNodo);
							}else if(i == socketYama){
								log_trace(log, "Se desconecto el YAMA");
								//printf("Se desconecto al YAMA \n");
							}

							close(i); // bye!
							FD_CLR(i, &master); // eliminar del conjunto maestro
						} else if(cabecera == BLOQUE){
							sem_wait(&binaryContenidoServidor);
							payload_BLOQUE * payload = data;

							pthread_mutex_lock(&mutexContenido);
							//string_append(&contenidoLeido, payload->contenido);

							FILE *archv = fopen(lecturaPathMD5, "a");
							fwrite(payload->contenido, 1, payload->tamanio_bloque, archv);
							fclose(archv);
							free(payload->contenido);
							//free(payload);

							pthread_mutex_unlock(&mutexContenido);
							log_trace(log,"Leido OK bloque %i \n", payload->numero_bloque);

							sem_post(&binaryContenidoConsola);
						} else if(cabecera == ALMACENAR_ARCHIVO){
							payload_ALMACENAR_ARCHIVO * payload = data;
							log_error(log, "Destino: %s, Name: %s, Tipo: %s, Tamanio %i", payload->pathDestino, payload->nombre, payload->tipo, payload->tamanio_contenido);
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

	if(bloquesLibres[0].bloque != -1){ // TODO
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

		free(nombreBloque);
		free(almacenamientoBloque);
	}


	if(list_size(listaDeNodos) > 1){ //TODO validar que no sea -1
		// Envio la copia

		if(bloquesLibres[1].bloque != -1){
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

			free(nombreBloque);
			free(almacenamientoBloque);
		}


	}

	// Libero la estructura
	free(bloquesLibres);

	return 0;

}

static t_list *cortar_modo_texto(FILE *in){
	t_list *retVal = list_create();

	int n;
	char *buffer = malloc(tamanioBloques);
	int offset = 0;
	int j = 0;
	while ( (n=fread(buffer,1,tamanioBloques,in)) > 0 ) {

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
		char* textoACopiar = string_substring(buffer, 0, j);
		memcpy(nodo->contenido, textoACopiar, j);
		free(textoACopiar);
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
		return FRACASO;
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
					log_error(log, "No se encontro el archivo");
					return FRACASO;
				}

				lista_de_paginas = cortar_modo_texto(in);

			}
			else{
				if ( (in = fopen(pathOrigenCompleto, "rb") ) == NULL ) {
					log_error(log,"No se encontro el archivo");
					return FRACASO;
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
				for ( i=0; i<list_size(lista_de_paginas); i++){
					t_pagina *pagina = list_get(lista_de_paginas, i);
					free(pagina->contenido);
					free(pagina);
					//free(bloqueNro);
				}
			}
			list_destroy(lista_de_paginas);

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


	return EXITO;
}

int almacenarArchivoWorker(char* pathDestino, char *name, char *tipo, char *contenido, int tamanioContenido, int socketRecibido){
	//log_error(log, "Destino: %s, Name: %s, Tipo: %s, Tamanio %i", pathDestino, name, tipo, tamanioContenido);

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
			free(contenido);



			// Creo una lista de paginas donde almaceno estructuras de tipo t_pagina
			// Refleja el archivo leido
			// En el caso de binario, todos los bloques miden lo mismo salvo el ultimo que puede medir menos
			// En el caso de texto, cada bloque mide 1MB o menos

			FILE *in;

			t_list *lista_de_paginas;

			if ( strcmp(tipo,"txt") == 0 ) {
				if ( (in = fopen(pathTemp, "r") ) == NULL ) {
					log_error(log, "No se encontro el archivo");
					return FRACASO;
				}

				lista_de_paginas = cortar_modo_texto(in);

			}
			else{
				if ( (in = fopen(pathTemp, "rb") ) == NULL ) {
					log_error(log, "No se encontro el archivo");
					return FRACASO;
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

				log_info(log, "Fin almacenamiento. Enviando EXITO a Master");
				send_EXITO_OPERACION(socketRecibido);
				log_info(log, "EXITO operacion enviado");
			}else{
				log_error(log, "No hay suficiente espacio para almacenar el archivo.");
				for ( i=0; i<list_size(lista_de_paginas); i++){
					t_pagina *pagina = list_get(lista_de_paginas, i);
					free(pagina->contenido);
					free(pagina);
					//free(bloqueNro);
				}

				send_FRACASO_OPERACION(socketRecibido);
				log_info(log, "Enviado fracaso a master");
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



		}else{
			send_FRACASO_OPERACION(socketRecibido);
		}


	}


	return EXITO;
}



void enviarAYama(int numNodo, int bloqueDelNodo, int bloqueDelArchivo, int copia, char *ipDatanode, int tamanioBloque){
	send_UBICACION_BLOQUE(socketYama, ipDatanode , 5042 , numNodo, bloqueDelNodo, bloqueDelArchivo, copia, tamanioBloque);
}

int leerArchivo(char *pathConNombre){
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
		FILE *in;
		if ( (in = fopen(pathArchivoConfig, "r") ) == NULL ) {
			log_error(log, "No se encontro el archivo");

			send_FRACASO_OPERACION(socketYama);
		}else{
			t_config* archivo_configuracion = config_create(pathArchivoConfig);

			char **nodoYBloque;
			char **nodoYBloqueCopia;

			int ok = 1;
			int i = 0;
			while(ok == 1){

				char *copiaCero = string_from_format("BLOQUE%iCOPIA0", i);
				char *copiaUno = string_from_format("BLOQUE%iCOPIA1", i);

				if(config_has_property(archivo_configuracion ,copiaCero)){
					char *getConfig = config_get_string_value(archivo_configuracion, copiaCero);
					nodoYBloque = string_get_string_as_array(getConfig);
					log_info(log, "Leido de %s -- bloque %s -- ORDEN %i -- Original \n", nodoYBloque[0], nodoYBloque[1], i);
					free(getConfig);
					// Agarro el tamanio del bloque
					char *bloquesBytes = string_from_format("BLOQUE%iBYTES", i);
					int tamanioBloque = config_get_int_value(archivo_configuracion, bloquesBytes);
					free(bloquesBytes);

					// Se envia a YAMA
					char *nombreNodo = string_substring_from(nodoYBloque[0],4);
					int nroNodo = atoi(nombreNodo);
					free(nombreNodo);
					enviarAYama(nroNodo, atoi(nodoYBloque[1]), i, 0, getIpNodoByName(nroNodo), tamanioBloque);
					free(nodoYBloque);
				}

				if(config_has_property(archivo_configuracion ,copiaUno)){
					char *getConfig = config_get_string_value(archivo_configuracion, copiaUno);
					nodoYBloqueCopia = string_get_string_as_array(config_get_string_value(archivo_configuracion, copiaUno));
					free(copiaUno);
					log_info(log, "Leido de %s -- bloque %s -- ORDEN %i -- Copia \n", nodoYBloqueCopia[0], nodoYBloqueCopia[1], i);

					// Agarro el tamanio del bloque
					char *bloquesBytes = string_from_format("BLOQUE%iBYTES", i);
					int tamanioBloque = config_get_int_value(archivo_configuracion, bloquesBytes);
					free(bloquesBytes);

					// Se envia a YAMA
					int nroNodoCopia = atoi(string_substring_from(nodoYBloqueCopia[0],4));
					enviarAYama(nroNodoCopia, atoi(nodoYBloqueCopia[1]), i, 1, getIpNodoByName(nroNodoCopia), tamanioBloque);
					free(nodoYBloqueCopia);
				}

				if(!config_has_property(archivo_configuracion ,copiaCero) && !config_has_property(archivo_configuracion ,copiaUno)){
					if(i == 0){
						log_error(log, "No se encontro el archivo en la ruta indicada");
						return FRACASO;
					}

					ok = 0;
				}
				//free(copiaCero);
				//free(copiaUno);
				i++;
			}

			//free(nodoYBloque);
			//free(nodoYBloqueCopia);
			free(archivo_configuracion);


			fclose(in);
			send_FIN_LISTA(socketYama);
		}
		free(indicePath);
		free(arrayPath);
		free(name);
	}
	return EXITO;

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
			perror("No se encontro el archivo \n");
		}else{
			t_config* archivo_configuracion = config_create(pathArchivoConfig);

			char **nodoYBloque;
			char **nodoYBloqueCopia;

			if(config_has_property(archivo_configuracion ,"TAMANIO")){
				printf("TAMANIO: %i \n", config_get_int_value(archivo_configuracion ,"TAMANIO"));
			}
			if(config_has_property(archivo_configuracion ,"TIPO")){
				printf("TIPO: %s \n", config_get_string_value(archivo_configuracion ,"TIPO"));
			}

			int ok = 1;
			int i = 0;
			while(ok == 1){
				char *copiaCero = string_from_format("BLOQUE%iCOPIA0", i);
				char *copiaUno = string_from_format("BLOQUE%iCOPIA1", i);

				char *bloquesBytes = string_from_format("BLOQUE%iBYTES", i);
				if(config_has_property(archivo_configuracion ,bloquesBytes)){
					printf("Bloque %i \n", i);
					printf("Tamanio bloque: %i \n", config_get_int_value(archivo_configuracion, bloquesBytes));
				}
				free(bloquesBytes);


				if(config_has_property(archivo_configuracion ,copiaCero)){
					nodoYBloque = string_get_string_as_array(config_get_string_value(archivo_configuracion, copiaCero));
					printf("ORIGINAL: %s -- Bloque %s \n", nodoYBloque[0], nodoYBloque[1], i);
					free(nodoYBloque);
				}

				if(config_has_property(archivo_configuracion ,copiaUno)){
					nodoYBloqueCopia = string_get_string_as_array(config_get_string_value(archivo_configuracion, copiaUno));
					log_info(log, "COPIA: %s -- Bloque %s\n", nodoYBloqueCopia[0], nodoYBloqueCopia[1], i);
					free(nodoYBloqueCopia);
				}

				if(!config_has_property(archivo_configuracion ,copiaCero) && !config_has_property(archivo_configuracion ,copiaUno)){
					ok = 0;
				}
				free(copiaCero);
				free(copiaUno);

				i++;
			}

			config_destroy(archivo_configuracion);
			fclose(in);
		}
		free(indicePath);
		free(arrayPath);
		free(name);
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

void renameDirectory(int indice, char *newName){
	free(tablaDeDirectorios[indice].nombre);
	strcpy(tablaDeDirectorios[indice].nombre, newName);
	free(newName);
	saveTablaDeDirectorios();

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


int leerContenidoArchivo(char *pathConNombre){

	if(esRutaYamaFSConNombre(pathConNombre)==1){
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

		if(fopen(lecturaPathMD5, "r")){
			remove(lecturaPathMD5);
		}

		FILE *in;
		if ( (in = fopen(pathArchivoConfig, "r") ) == NULL ) {
			log_error(log, "No se encontro el archivo");
		}else{
			t_config* archivo_configuracion = config_create(pathArchivoConfig);

			char **nodoYBloque;
			char **nodoYBloqueCopia;
			int socketOriginal = -1;
			int socketCopia = -1;
			int ok = 1;
			int i = 0;
			while(ok == 1){
				char *copiaCero = string_from_format("BLOQUE%iCOPIA0", i);
				char *copiaUno = string_from_format("BLOQUE%iCOPIA1", i);

				if(config_has_property(archivo_configuracion ,copiaCero)){
					nodoYBloque = string_get_string_as_array(config_get_string_value(archivo_configuracion, copiaCero));
					// Agarro el tamanio del bloque
					char *bloquesBytes = string_from_format("BLOQUE%iBYTES", i);
					int tamanioBloque = config_get_int_value(archivo_configuracion, bloquesBytes);
					free(bloquesBytes);
					char *nombreNodo = string_substring_from(nodoYBloque[0],4);
					socketOriginal = getSocketNodoByName(atoi(nombreNodo));
					if(socketOriginal != -1){
						// Pido el original
						log_info(log, "Pedido a %s -- bloque %s -- ORDEN %i -- Original || Tam: %i \n", nombreNodo , nodoYBloque[1], i, tamanioBloque);
						send_PETICION_BLOQUE(socketOriginal,atoi(nodoYBloque[1]), tamanioBloque);
						sem_post(&binaryContenidoServidor);
						sem_wait(&binaryContenidoConsola);

					}
					free(nombreNodo);
					free(nodoYBloque);
				}

				if(config_has_property(archivo_configuracion ,copiaUno) && socketOriginal == -1){
					nodoYBloqueCopia = string_get_string_as_array(config_get_string_value(archivo_configuracion, copiaUno));

					// Agarro el tamanio del bloque
					char *bloquesBytes = string_from_format("BLOQUE%iBYTES", i);
					int tamanioBloque = config_get_int_value(archivo_configuracion, bloquesBytes);
					free(bloquesBytes);

					int socketCopia = getSocketNodoByName(atoi(string_substring_from(nodoYBloqueCopia[0],4)));
					if(socketCopia != -1){
						// Pido la copia
						log_info(log, "Pedido a %s -- bloque %s -- ORDEN %i -- Copia \n", string_substring_from(nodoYBloqueCopia[0],4) , nodoYBloqueCopia[1], i);
						send_PETICION_BLOQUE(socketCopia,atoi(nodoYBloqueCopia[1]), tamanioBloque);
						sem_post(&binaryContenidoServidor);
						sem_wait(&binaryContenidoConsola);

					}
					free(nodoYBloqueCopia);
				}


				if(!config_has_property(archivo_configuracion ,copiaCero) && !config_has_property(archivo_configuracion ,copiaUno)){
					ok = 0;
				}

				if(ok != 0 && socketOriginal == -1 && socketCopia == -1){
					//free(nodoYBloque);
					//free(nodoYBloqueCopia);

					fclose(in);
					config_destroy(archivo_configuracion);
					log_error(log, "Los sockets no estan disponibles");
					fprintf(stderr, "No se encontraron nodos disponibles \n");
					return FRACASO;
					ok = 0;
				}

				free(copiaCero);
				free(copiaUno);
				i++;
			}

			config_destroy(archivo_configuracion);
			fclose(in);
		}
		free(indicePath);
		free(arrayPath);
		free(name);

		return EXITO;
	}else{
		return FRACASO;
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
		//printf("El sistema se encuentra recien formateado ==> ESTABLE \n" );
		return 1;
	}else{
		int estable = 1;

		//char *stringNodosConectados = string_new();
		char *stringNodosConectados = listaDeNodosAsArray();

		//char **nodosConectados = string_get_string_as_array(stringNodosConectados);

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

		if(estable == 1){
			log_trace(log, "El sistema se encuentra en estado ESTABLE");
			//printf("El sistema se encuentra ESTABLE \n" );
		}else{
			log_trace(log, "El sistema se encuentra en estado NO ESTABLE");
			//printf("El sistema se encuentra NO ESTABLE \n" );
		}


		free(stringNodosConectados);
		return estable;
	}


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
							log_trace(log, "%s\n", archivos->d_name);

							t_config* currFile = config_create(string_from_format("%s%s%s/%s", directorioRaiz, pathArchivos, dir->d_name, archivos->d_name));

							int ok = 1;
							int i = 0;
							while(ok == 1){
								char *copiaCero = string_from_format("BLOQUE%iCOPIA0", i);
								char *copiaUno = string_from_format("BLOQUE%iCOPIA1", i);


								if(config_has_property(currFile ,copiaCero)){
									//char **nodoYBloque = malloc(sizeof(char*)*2);
									char **nodoYBloque = string_get_string_as_array(config_get_string_value(currFile, copiaCero));
									char *nombreNodo = string_substring_from(nodoYBloque[0],4);
									log_info(log,"Necesito nodo %i para la copia 0 \n", atoi(nombreNodo));
									free(nombreNodo);

									agregarNodoAListaSiNoExiste(nodosParaOriginal, string_duplicate(nodoYBloque[0]));


									free(nodoYBloque);

								}

								if(config_has_property(currFile ,copiaUno)){
									//char **nodoYBloque = malloc(sizeof(char*)*2);
									char** nodoYBloque = string_get_string_as_array(config_get_string_value(currFile, copiaUno));
									char *nombreNodo = string_substring_from(nodoYBloque[0],4);
									log_info(log, "Necesito nodo %i para la copia 1 \n", atoi(nombreNodo));
									free(nombreNodo);

									agregarNodoAListaSiNoExiste(nodosParaCopia, string_duplicate(nodoYBloque[0]));

									free(nodoYBloque);
								}

								if(!config_has_property(currFile , copiaCero) && !config_has_property(currFile , copiaUno)){
									ok = 0;
								}
								free(copiaCero);
								free(copiaUno);

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
		//printf("No se encontro el archivo de directorios. No hay un estado anterior para restaurar.");
		return 1;
	}
	else if(stat(pathTablaNodos, &st) == -1){
		log_info(log, "No se encontro la tabla de nodos. No hay un estado anterior para restaurar.");
		//printf("No se encontro la tabla de nodos. No hay un estado anterior para restaurar.");
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
		//printf("Se encontro un estado anterior. Esperando nodos...");
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

	if(remove(PATHDIRECTORIOS) == -1){
		//No se elimino
		log_error(log,"No se pudo eliminar la tabla de archivos");
	}

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
	log_trace(log, "bloque %i\n",bloque);
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
		log_trace(log, "%i", a);
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
	char *pathNewBitmap = string_duplicate(PATHBITMAP);
	//strcpy(pathNewBitmap, PATHBITMAP);
	string_append(&pathNewBitmap, "nodo");
	string_append(&pathNewBitmap, name);
	string_append(&pathNewBitmap, ".dat");
	free(name);

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
		log_info(log, "El bitmap del Nodo %i fue leido con exito.", nroNodo);
	}
	free(pathNewBitmap);
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
		fprintf(stderr, "No se encontro el directorio \n");
	}
	else{
		int i;
		for (i = 0; i < TOTALDIRECTORIOS; i++) {
			if(tablaDeDirectorios[i].padre == indice){
				printf("%s ", tablaDeDirectorios[i].nombre );
			}

		}
		//log_trace(log, "\n");
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
	printf("\n");
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

			log_error(log, "La ruta indicada no es una ruta valida en YamaFs");
			fprintf(stderr, "La ruta indicada no es una ruta valida en YamaFs \n");
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

void createLogicDirectory(char* path){
	if(string_starts_with(path, "/")){
		log_error(log, "Error al crear directorio. Se encuentra en root/");
		fprintf(stderr, "Error al crear directorio. Se encuentra en root/ \n");
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
				fprintf(stderr, "La tabla de directorios esta completa \n");

			}else{
				char **padres = string_split(path, "/");
				//int cant;
				//cant = strlen(padres) / sizeof(char*); //Length de padres
				int cant = 0;
				while(padres[cant] != NULL ){
					cant++;
				}


				if(findDirByname(padres[cant-1]) == -1){ //Si es -1 no existe

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
					printf("El directorio %s fue creado con exito \n", pathConcat);

					free(padres);
				}else{
					log_error(log, "El directorio que intenta crear ya existe");
					fprintf(stderr, "El directorio que intenta crear ya existe \n");
				}

			}
			saveTablaDeDirectorios();
	}


}

int deleteDirectory(char* path){
	if(string_starts_with(path, "/")){
		log_error(log, "Error al eliminar directorio. Se encuentra en root/");
		return FRACASO;
	}else{
		char* pathConcat = string_new();
		string_append(&pathConcat, directorioRaiz);
		string_append(&pathConcat, path);

		struct stat st = {0};

		if (stat(pathConcat, &st) == -1) { //Si no existe el path, no lo elimino
			log_error(log, "El directorio no existe");
			return FRACASO;
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
				log_info(log, "El directorio %s fue eliminado.\n", pathConcat);

			}
			else{
				fprintf(stderr, "No se pudo eliminar el archivo %s.\n", pathConcat);
				return FRACASO;
			}
		}

		saveTablaDeDirectorios();
		return EXITO;
	}

}
