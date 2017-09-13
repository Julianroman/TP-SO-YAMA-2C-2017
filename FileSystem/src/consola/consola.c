/*
 * consola.c
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "interfaces.h"


void ejecutar_comando(char* linea){
	// Puntero a la funcion interface que corresponda al comando
	void (*accion) ();
	// Divido el string en tokens, separando donde haya un espacio
	// El primer token sera el comando
	char* comando = strtok(linea," ");

	// Si el comando es reconocido
	if((accion = buscar_interfaz(comando))!=NULL){
		// ejecuto la interfaz del comando
		accion();
	}else{
		// Sino muestro un error
		printf("Comando desconocido\n");
	}
}

void init_consola(){
	char* linea = (char*) NULL;

	while(1){
		// Obtengo la linea desde el teclado
		linea = readline(">>");

		// Si la linea no esta vacia
	    if(linea){
	    	// la agrego al historial
	    	add_history(linea);
	    	// e intento ejecutar el comando que corresponda;
	    	ejecutar_comando(linea);
	    }
	}
};

