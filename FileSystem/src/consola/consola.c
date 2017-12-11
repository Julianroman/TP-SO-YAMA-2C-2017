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
	//char* comando = strtok(linea," ");
	char **comando = string_split(linea, " ");
	//if(string_contains(linea," ")){

		if((accion = buscar_interfaz(comando[0]))!=NULL){
			// ejecuto la interfaz del comando
			accion(comando);
		}else{
			// Sino muestro un error
			fprintf(stderr, "Comando no reconocido.\n");
		}
	}
	//else{
		/*comando = string_duplicate(linea);
		puts(comando);
		if((accion = buscar_interfaz(comando))!=NULL){
			// ejecuto la interfaz del comando
			accion(comando);
		}else{
			// Sino muestro un error
			printf("Comando desconocido\n");
		}*/
		/*if(strcmp(linea, "format") == 0){
			accion = buscar_interfaz(linea);
			accion(linea);
		}else{
			if((accion = buscar_interfaz(linea))!=NULL){
				fprintf(stderr, "Puede estar olvidando algo.\n");
			}else{
				fprintf(stderr, "Comando no reconocido.\n");
			}

		}*/

//	}

//}

void init_consola(){
	char* linea;

	while(1){
		// Obtengo la linea desde el teclado
		linea = readline("root/ >>");


		// Si la linea esta vacia
		if (!linea)
		        break;
		// Si la linea no esta vacia
		if(linea){
			// la agrego al historial
			add_history(linea);
			// e intento ejecutar el comando que corresponda;
			ejecutar_comando(linea);
			printf("\n");
		}

		free(linea);

	}
};

