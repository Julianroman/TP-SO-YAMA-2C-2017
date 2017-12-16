/*
 * ============================================================================
 * res_orden_almacenamiento.c
 * ============================================================================
 */

#include <stdio.h>
#include <utilidades/protocol/senders.h>
#include <utilidades/protocol/types.h>
#include <utilidades/protocol/receive.h>
#include <utilidades/socket_utils.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/wait.h>

#define BUFFERSIZE 1024
#define UNMB 1048576;
#define TAMANIOBLOQUE 1048576;

extern t_log* logger;

char *leerFinal(int size, char* path){
	int offset = 0;
	int archivo;
	char* lectura = malloc(size);
	archivo = open(path, O_RDONLY);
	char * map;
	if((map = mmap((caddr_t)0, size, PROT_READ, MAP_SHARED, archivo, offset)) == MAP_FAILED){
		log_error(logger,"No se pudo mappear archivo");
	}
	memcpy(lectura, map, size);
	if (munmap(map, size) == -1)
	{
		log_error(logger, "No se pudo liberar el map");
	}
	close(archivo);
	return lectura;
}

void res_ORDEN_ALMACENAMIENTO(int socket_cliente,HEADER_T header,void* data){
	log_info(logger, "Respondiendo ORDEN DE ALMACENAMIENTO");
	payload_ORDEN_ALMACENAMIENTO* orden = data;

	// Cargar el temporal local
	char* temporalPath = string_from_format("tmp/%s",orden->nombreTemporal_ReduccionGlobal);

		// Tamanio de archivo
	int input_file_size;
	FILE *input_file = fopen(temporalPath, "rb");
	fseek(input_file, 0, SEEK_END);
	input_file_size = ftell(input_file);
	fclose(input_file);

		//Obtener contenido
	char * contenido = leerFinal(input_file_size,temporalPath);


	// Mandar al FS
		// Recuperar metadata del archivo de configuracion
	t_config* archivo_configuracion = config_create("worker-config.cfg");
	int puertoFS = config_get_int_value(archivo_configuracion, "FS_PUERTO");
	char* ipFS = config_get_string_value(archivo_configuracion, "FS_IP");

	//Conectarse al FS
	log_info(logger,"Conectandose al FileSystem %s:%d",ipFS,puertoFS);
	int socketFS = crear_conexion(ipFS,puertoFS);
	config_destroy(archivo_configuracion);

	// Enviar contenido
	log_trace(logger,"RUTA:%s",orden->rutaAlmacenamiento);
	log_trace(logger,"NOMBRE:%s",orden->nombreAlamcenamiento);
	send_ALMACENAR_ARCHIVO(socketFS,input_file_size,contenido,orden->rutaAlmacenamiento,orden->nombreAlamcenamiento,"txt");
	free(contenido);

	// Esperar respuesta
	receive(socketFS,&header);

	if(header == EXITO_OPERACION){
		log_info(logger,"Job almacenado en %s/%s..",orden -> rutaAlmacenamiento, orden -> nombreAlamcenamiento);
		send_EXITO_OPERACION(socket_cliente);
	}else{
		log_error(logger,"No se pudo almacenar el resultado del Job");
		send_FRACASO_OPERACION(socket_cliente);
	}

	// Cerrar socket
	close(socketFS);
	exit(0);
};
