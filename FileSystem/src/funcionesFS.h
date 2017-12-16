/*
 * funcionesFS.h
 *
 *  Created on: 16/10/2017
 *      Author: utnso
 */

#ifndef FUNCIONESFS_H_
#define FUNCIONESFS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <commons/log.h>
#include <commons/config.h>
#include <sys/mman.h>
#include <commons/bitarray.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <utilidades/protocol/receive.h>
#include <utilidades/protocol/senders.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <semaphore.h>
#include <pthread.h>
#include <dirent.h>



#define TOTALDIRECTORIOS 100
#define PATHDIRECTORIOS "root/Directorios.txt"
#define PATHBITMAP "root/metadata/bitmaps/"

t_log *log; // Log file

typedef struct {
    int32_t nroNodo;
    t_bitarray* bitmap;
    int32_t socket;
    int32_t cantidadBloques;
    char *ipNodo;
} t_nodo;

t_nodo *nodo_create(int32_t nroNodo, t_bitarray* bitmap, int32_t socket, int32_t cantidadBloques, char *ip);

void nodo_destroy(t_nodo *self);

typedef struct {
	int32_t indice;
	char nombre[255];
	int32_t padre;
}t_directory;


// Estructura de pagina
typedef struct {
	char *contenido;
	size_t tamanio;
} t_pagina;

typedef struct{
	t_list *nodosOriginal;
	t_list *nodosCopia;
}t_nodos_por_archivo;

typedef struct {
	t_nodo *nodo;
	int bloque;
} t_bloque_libre;

void directory_destroy(t_directory *self);

void desconectarNodo(int id_dataNode);

void servidorFs(int puerto);

t_bloque_libre *traerBloquesLibres();

int enviarADataNode(t_pagina *unaPagina, t_config *fileExport, int nroBloque);

char* getLine(FILE* in);

int almacenarArchivo(char *location, char* pathDestino, char *name, char *tipo);

int almacenarArchivoWorker(char* pathDestino, char *name, char *tipo, char *contenido, int tamanioContenido, int socketRecibido);

void enviarAYama(int numNodo, int bloqueDelNodo, int bloqueDelArchivo, int copia, char *ipDatanode, int tamanioBloque);

int leerArchivo(char *pathConNombre);

void getInfoArchivo(char *pathConNombre);

int getSocketNodoByName(int nroNodo);

int leerContenidoArchivo(char *pathConNombre);

void agregarNodoAListaSiNoExiste(t_list *lista, char *nodo);

void nodosARestaurar();

int esEstadoEstable();

int existeEstadoAnterior();

void initOrRestoreFS();

void initFS();

void formatear();

void almacenarBitmapEnArchivo(t_nodo *unNodo);

int bloquesLibresEnNodo(t_nodo* unNodo);

void actualizarTablaDeNodos();

void actualizarNodoEnTabla(t_nodo *unNodo);

int cantidadTotalBloques();

int cantidadTotalBloquesLibres();

void escribirBloqueLibre(t_nodo* unNodo,int bloque);

int proximoBloqueLibre(t_nodo* unNodo);

void printBitmap(t_bitarray* unBitarray);

t_bitarray* initOrCreateBitmap(int nroNodo, int cantidadDeBloques);

char* listaDeNodosAsArray();

void agregarNodoATabla(t_nodo *unNodo);

void inicializarNodo(int nroNodo, int socket, int cantidadBloques,char *ipNodo);

void initTablaDeNodos();

void printLs(char* path);

void printTablaDeDirectorios();

void saveTablaDeDirectorios();

void initTablaDeDirectorios();

int findDirByname(char* name);

int esRutaYamaFS(char* path);

int esRutaYamaFSConNombre(char *pathConNombre);

void createDirectory(char* path);

int deleteDirectory(char* path);

void renameDirectory(int indice, char *newName);

void moveDirectory(char* rutaInicial, char* rutaFinal);

int getIndiceNodoBySocket(int socket);

char *getIpNodoByName(int name);

#endif /* FUNCIONESFS_H_ */
