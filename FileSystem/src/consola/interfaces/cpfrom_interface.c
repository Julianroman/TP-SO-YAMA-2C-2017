/*
 * cpfrom_interface.c
 */

#include <stdio.h>
//cpfrom [path_archivo_origen] [directorio_yamafs]
void cpfrom_interface(char **comando){
	copiaLocalAlYamafs(comando[1], comando[2]);
};
