/*
 * res_procesamiento.c
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <commons/collections/dictionary.h>
#include "../responses.h"

YAMA_STATUS res_procesamiento(int socket,t_dictionary* diccionario){
	printf("Procesamiento iniciado ");
	payload_solicitud_procesamiento payload;
	recv(socket, &payload, sizeof payload, 0);
	printf("para archivo %s \n",payload.nombreArchivo); // Esto no funciona evidentemente, pero se solucionara cuando se implemente serializacion
														// De cualquier modo, se recibe correctamente, solo que no se interpreta bien

	// Funciones de diccionario
	// TODO desarrollar las utilidades del diccionario de master
	char key = (char) socket;
	struct dummie_data{
		char* nomArch;
	}data;
	data.nomArch= payload.nombreArchivo;
	dictionary_put(diccionario, &key, &data);
	void *retorno = dictionary_get(diccionario,&key);
	char* archFromDic = ((struct dummie_data *) retorno)->nomArch;
	printf("En diccionario/%d : %s \n",key,archFromDic);
	return EXITO;
};

