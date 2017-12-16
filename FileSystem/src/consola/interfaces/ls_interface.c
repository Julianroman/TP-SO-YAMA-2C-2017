/*
 * ls_interface.c
 */

#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>

void ls_interface(char** comando){
	int cant = 0;
	while(comando[cant] != NULL){
		cant++;
	}

	if(cant != 2){
		fprintf(stderr, "Comando erroneo. Podria probar con: ls [path_carpeta] \n");
	}else{

		if(esRutaYamaFS(comando[1])==1){

			char **pathSplit = string_split(comando[1], "/");
			int cant = 0;
			while(pathSplit[cant] != NULL){
				cant ++;
			}
			int indice = findDirByname(pathSplit[cant - 1]);

			char *ruta = string_from_format("root/metadata/archivos/%i/", indice);

			DIR *mydir;
			struct dirent *myfile;
			struct stat mystat;
			char buf[512];
			mydir = opendir(ruta);
			if(mydir == NULL){
				//fprintf(stderr, "No se encontro el directorio %s\n", ruta);
				//no esta la carpeta con indice

			}
			else{
				while((myfile = readdir(mydir)) != NULL)
					{
						if(!string_equals_ignore_case(myfile->d_name, ".") && !string_equals_ignore_case(myfile->d_name, "..")){
							sprintf(buf, "%s/%s", comando[1], myfile->d_name);
							stat(buf, &mystat);
							//printf("%d",mystat.st_size); //Tamanio del archivo
							printf(" %s ", myfile->d_name); //Nombre del archivo
						}

					}
			}
			closedir(mydir);

			printLs(comando[1]);
			printf("\n");
		}



	}

};
