/*
 ============================================================================
 Name        : YAMA.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "serverYAMA/responses.h"
#include "serverYAMA/serverYAMA.h"
#include "YAMA.h"

static int idUltimoMasterCreado = 1;
#define BACKLOG       5


void leerConfiguracion(){
	char* path = "/home/utnso/workspace/tp-2017-2c-Grupo-1---K3525/YAMA/src/yama-config.cfg";
	t_config* archivo_configuracion = config_create(path);
	configYAMA = malloc(sizeof(t_yama));
	configYAMA->FS_PUERTO = config_get_int_value(archivo_configuracion, "FS_PUERTO");
	printf("El puerto FS es: %d \n", configYAMA->FS_PUERTO);

	configYAMA->FS_IP = config_get_string_value(archivo_configuracion, "FS_IP");
	printf("La IP FS es: %s \n", configYAMA->FS_IP);

	configYAMA->retardoPlanificacion = config_get_int_value(archivo_configuracion, "RETARDO_PLANIFICACION");
	printf("El retardo de la Planificacion es: %i \n", configYAMA->retardoPlanificacion);

	configYAMA->algoritmoBalanceo = config_get_string_value(archivo_configuracion, "ALGORITMO_BALANCEO");
	printf("El Algoritmo de Balanceo es: %s \n", configYAMA->algoritmoBalanceo);

	configYAMA->ipYAMA = config_get_string_value(archivo_configuracion, "YAMA_IP");
	printf("YAMA se conectara la IP: %s \n", configYAMA->ipYAMA);

	configYAMA->puertoYAMA = config_get_int_value(archivo_configuracion, "YAMA_PUERTO");
	printf("YAMA se conectara en el puerto: %d \n", configYAMA->puertoYAMA);

	configYAMA->base = config_get_int_value(archivo_configuracion, "BASE");
	printf("La disponibilidad base es: %d \n", configYAMA->base);
}


int main(void) {
	puts("Comienza el proceso YAMA");
	logYAMA = log_create("yama.log", "YAMA", true, LOG_LEVEL_TRACE);
	log_trace(logYAMA, "Leyendo configuracion");
	leerConfiguracion();
	log_trace(logYAMA, "Configuracion leida");


	TablaEstados = list_create();
	MastersJobs = list_create();

	pthread_t hiloWorkers;
	pthread_create(&hiloWorkers, NULL, (void*) init_serverYAMA, configYAMA->puertoYAMA);
	log_trace(logYAMA, "Iniciando servidor YAMA");

	pthread_t hiloFS;
	pthread_create(&hiloFS, NULL, (void*) clienteFS, NULL);

	pthread_join(hiloFS, NULL);
	pthread_join(hiloWorkers, NULL);


	return EXIT_SUCCESS;
}

void clienteFS(){
	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = inet_addr(configYAMA->FS_IP);
	direccionServidor.sin_port = htons(configYAMA->FS_PUERTO);

	int cliente = socket(AF_INET, SOCK_STREAM, 0);
	if (connect(cliente, (void*) &direccionServidor, sizeof(direccionServidor)) != 0) {
		perror("No se pudo conectar");
		exit(1);
	}

	//------- Mensaje de bienvenida del FileSystem ---------------
	char* buffer = malloc(1000);
	int bytesRecibidos = recv(cliente, buffer, 1000, 0);
	buffer[bytesRecibidos] = '\0';
	printf("FileSystem dice: %s\n", buffer);
	free(buffer);
	//------------------------------------------------------------

	//Aca se queda escuchando para recibir bloques
	HEADER_T cabecera;
	void* data;
	// wait worker
	send_PETICION_NODO(cliente, "/home/utnso/workspace/tp-2017-2c-Grupo-1---K3525/FileSystem/root/metadata/archivoBinario.bin");
	while (1) {
			data = receive(cliente,&cabecera);
			if(cabecera == UBICACION_BLOQUE){
				log_trace(logYAMA, "Escuche a FS");
			}
			if(cabecera == FIN_LISTA){
				log_trace(logYAMA, "Escuche a FS");
			}
	}
};

void servidorYAMA(int puerto){

	fd_set master, masterAux; // conjunto maestro de descriptores de fichero y uno auxiliar para el select()
	FD_ZERO(&master);    // borra los conjuntos maestro y temporal
	FD_ZERO(&masterAux);

	// obtener socket a la escucha
	int servidor = socket(AF_INET, SOCK_STREAM, 0);
	if (servidor == -1) {
		perror("Falló el socket");
		exit(1);
	}
	// obviar el mensaje "address already in use" (la dirección ya se está usando)
	int activado = 1;
	int socapete = setsockopt(servidor, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));
	if (socapete == -1) {
		perror("Falló el setsockopt");
		exit(1);
	}

	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = INADDR_ANY;
	direccionServidor.sin_port = htons(puerto);
	memset(&(direccionServidor.sin_zero), '\0', 8);

	if (bind(servidor, (void*) &direccionServidor, sizeof(direccionServidor)) != 0) {
		perror("Falló el bind");
		exit(1);
	}
	// escuchar
	printf("Esperando conexiones...\n");
	if (listen(servidor, 100) == -1) {
		perror("Falló el listen");
		exit(1);
	}

	// añadir servidor al conjunto maestro
	FD_SET(servidor, &master);
	// seguir la pista del descriptor de fichero mayor
	int fdmax = servidor; // por ahora es éste
	struct sockaddr_in direccionCliente; // dirección del cliente
	//char* buffer = malloc(1000);
	// bucle principal
	while(1) {
		masterAux = master; // cópialo
		if (select(fdmax+1, &masterAux, NULL, NULL, NULL) == -1) {
			perror("Falló el select");
			exit(1);
		}
		// explorar conexiones existentes en busca de datos que leer
		int i;
		for(i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &masterAux)) { // ¡¡tenemos datos!!
				if (i == servidor) {
					int addrlen = sizeof(direccionCliente);
					int cliente = accept(servidor, (void*) &direccionCliente, &addrlen);
					if (cliente  == -1) {
						perror("Falló el accept");
						exit(1);
					} else {
						FD_SET(cliente, &master); // añadir al conjunto maestro
						if (cliente > fdmax) {    // actualizar el máximo
							fdmax = cliente;
						}
						/*char* mensaje = "Bienvenido a FS!!";
						send(cliente, mensaje, strlen(mensaje), 0);*/
						log_trace(logYAMA, "Se conecto Master %d", idUltimoMasterCreado);
						agregarAListado(idUltimoMasterCreado, cliente);
					}
				}
				} else {
					HEADER_T cabecera;
					void* data;
					data = receive(i,&cabecera);
					if (cabecera == FIN_COMUNICACION){ //Si header es FIN_COMUNICACION es porque se cerro la conexion
						FD_CLR(i,&master); // Eliminar de la lista
						break;
					}
					else{
						responder_SOLICITUD(i, data, cabecera); // Responder solicitud
					}
				} // Esto es ¡TAN FEO!
			}
	}
}

