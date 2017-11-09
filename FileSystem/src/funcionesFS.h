/*
 * funcionesFS.h
 *
 *  Created on: 16/10/2017
 *      Author: utnso
 */

#ifndef FUNCIONESFS_H_
#define FUNCIONESFS_H_

//#include <utilidades/Sockets.c>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <commons/log.h>
#include <commons/config.h>
#include <pthread.h>
#include <sys/mman.h>
#include <commons/bitarray.h>
#include <commons/collections/list.h>
#include <utilidades/protocol/receive.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


#define TOTALDIRECTORIOS 100
#define PATHDIRECTORIOS "/home/utnso/Directorios.txt"
#define PATHBITMAP "root/metadata/bitmaps/"



typedef struct {
    int32_t nroNodo;
    t_bitarray* bitmap;
    int32_t socket;
    int32_t cantidadBloques;
} t_nodo;

t_nodo *nodo_create(int32_t nroNodo, t_bitarray* bitmap, int32_t socket, int32_t cantidadBloques);

void nodo_destroy(t_nodo *self);

typedef struct {
	int32_t indice;
	char nombre[255];
	int32_t padre;
}t_directory;

/*static t_directory *directory_create(int32_t index, char* nombre, int32_t padre) {
	t_directory *new = malloc(sizeof(t_directory));
    new->indice = index;
    new->nombre = nombre;
    new->padre = padre;

    return new;
}*/

void directory_destroy(t_directory *self);


void servidorFs(int puerto);


void copiaLocalAlYamafs(char* pathOrigen, char* pathDestino);


int enviarADataNode(char* map, int bloque, int tam, int size_bytes);

int almacenarArchivo(char* location, char* destino, char* tipo);

void formatear();

void almacenarBitmapEnArchivo(t_nodo *unNodo);

int bloquesLibresEnNodo(t_nodo* unNodo);

int cantidadTotalBloquesLibres();

void escribirBloqueLibre(t_nodo* unNodo,int bloque);

int proximoBloqueLibre(t_nodo* unNodo);

void printBitmap(t_bitarray* unBitarray);

t_bitarray* initOrCreateBitmap(int nroNodo, int cantidadDeBloques);

void inicializarNodo(int nroNodo, int socket, int cantidadBloques);

void printLs(char* path);

void printTablaDeDirectorios();

void saveTablaDeDirectorios();

void initTablaDeDirectorios();

int findDirByname(char* name);

void createDirectory(char* path);

void deleteDirectory(char* path);



#endif /* FUNCIONESFS_H_ */
