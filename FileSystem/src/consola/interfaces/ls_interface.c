/*
 * ls_interface.c
 */

#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>

void ls_interface(char** comando){
	/*DIR *mydir;
	struct dirent *myfile;
	struct stat mystat;

	char buf[512];
	mydir = opendir(comando[1]);
	if(mydir == NULL){
		fprintf(stderr, "No se encontro el directorio %s\n", comando[1]);
	}
	else{
		while((myfile = readdir(mydir)) != NULL)
			{
				sprintf(buf, "%s/%s", comando[1], myfile->d_name);
				stat(buf, &mystat);
				printf("%d",mystat.st_size); //Tamanio del archivo
				printf(" %s\n", myfile->d_name); //Nombre del archivo
			}
	}

	closedir(mydir);*/
	printLs(comando[1]);
};
