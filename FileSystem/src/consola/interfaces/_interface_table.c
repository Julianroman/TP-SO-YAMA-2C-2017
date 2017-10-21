/*
 * interface.c
 */

#include <stddef.h>
#include <string.h>
#include "../interfaces.h"

// Cantidad de elementos de un array x
#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))

// Tabla de interfaces
typedef struct {
	char* comando;
	void* interfaz;
}fila_tabla_comandos;

fila_tabla_comandos tablaDeComandos[]={{"format"  ,&format_interface},
									   {"rm"      ,&rm_interface}, //TODO flags
									   {"rename"  ,&rename_interface}, //OK
									   {"mv"      ,&mv_interface},
									   {"cat"     ,&cat_interface},
									   {"mkdir"  , &mkdir_interface}, //OK
									   {"cpfrom"  ,&cpfrom_interface}, //OK
									   {"cpto"    ,&cpto_interface},
									   {"cpblock" ,&cpblock_interface},
									   {"md5"     ,&md5_interface},
									   {"ls"      ,&ls_interface}, //OK (creo)
									   {"info"    ,&info_interface}};
/**/

// Macheo comando/interfaz
void* buscar_interfaz(char* comando){
	int i, n = NELEMS(tablaDeComandos);
	for(i=0; i<n; i++){
		if(strcmp(comando,tablaDeComandos[i].comando) == 0){
			return tablaDeComandos[i].interfaz;
		}
	}
	return NULL;
}
/**/
