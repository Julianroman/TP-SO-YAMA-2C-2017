/*
 ============================================================================
 Name        : DataNode.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <parser/metadata_program.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <commons/string.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int puertoFs = 0;
char* ipFs = "";
t_log* log;

void leerConfiguracion(){
	char* path = "/home/utnso/workspace/tp-2017-2c-Grupo-1---K3525/DataNode/src/nodo-config.cfg";
	t_config* archivo_configuracion = config_create(path);
	puertoFs = config_get_int_value(archivo_configuracion, "PUERTO_FILESYSTEM");
	printf("El puerto FS es: %i \n", puertoFs);
	ipFs = config_get_string_value(archivo_configuracion, "IP_FILESYSTEM");
	printf("La IP FS es: %s \n", ipFs);

	config_destroy(archivo_configuracion);
}

int main(void) {
	puts("Comienza DataNode");
	//Se crea el log
	log = log_create("dataNode.log", "DataNode", false, LOG_LEVEL_TRACE);
	log_trace(log, "Leyendo configuracion");
	leerConfiguracion();
	log_trace(log, "Configuracion leida");

	struct sockaddr_in direccionServidor;
		direccionServidor.sin_family = AF_INET;
		direccionServidor.sin_addr.s_addr = inet_addr("127.0.0.1");
		direccionServidor.sin_port = htons(puertoFs);

		int cliente = socket(AF_INET, SOCK_STREAM, 0);
		if (connect(cliente, (void*) &direccionServidor, sizeof(direccionServidor)) != 0) {
			perror("No se pudo conectar");
			return 1;
		}

		while (1) {
			char mensaje[1000];
			scanf("%s", mensaje);
			send(cliente, mensaje, strlen(mensaje), 0);
		}

	return EXIT_SUCCESS;
}
