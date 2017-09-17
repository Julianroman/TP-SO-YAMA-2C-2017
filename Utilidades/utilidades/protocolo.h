/*
 * protocolo.h
 *
 *  Created on: 7/9/2017
 *      Author: utnso
 */

#ifndef PROTOCOLO_H_
#define PROTOCOLO_H_
#define TAMANIO_RUTA 256
#define TAMANIO_NOMBRE 64
#define TAMANIO_MSG  64
#define TAMANIO_IPPUERTO  32

typedef enum mensaje{SOLICITUDPROCESAMIENTO, MSGESTADO, INFOWORKERTRANS,
     ORDENTRANS, ENVIOEXEC, INFOWORKERREDUXLOC, ORDENREDUXLOC,
     INFOWORKERGLOBREDUXGLOB, ORDENREDUXGLOB, INFOWORKERALMA,
     ORDENALMA, ADQUISBLOQ, BLOQUE, ESCRITURABLOQ, ALMACENARCH,
     LEERARCH, ARCHIVO} t_Mensaje;


/*
================================================================
 <char> Descipcion
================================================================
 */

/*
================================================================
 < 0 >  Solicitud de procesamiento
================================================================
 */
typedef struct{
	char nombreArchivo[TAMANIO_RUTA];
}payload_solicitud_procesamiento;
typedef struct{
	char header;
	payload_solicitud_procesamiento payload;
}paquete_solicitud_procesamiento;

/*
================================================================
 < 1 >  Mensaje de estado
================================================================
 */
typedef struct{
	char mensajeStatus[TAMANIO_MSG];
}payload_mensaje_de_estado;
typedef struct{
	char header;
	payload_solicitud_procesamiento payload;
}paquete_mensaje_de_estado;

/*
================================================================
 < 2 >  Informacion de workers (transformacion)
================================================================
 */
typedef struct{
	char ip_puerto[TAMANIO_IPPUERTO];
	int  bloque;
	int  bytesOcupados;
	char nombreArchivoTemporal[TAMANIO_NOMBRE];
}payload_paquete_info_workers_transformacion;
typedef struct{
	char header;
	payload_paquete_info_workers_transformacion payload;
}paquete_info_workers_transformacion;

/*
================================================================
 < 3 >  Orden de transformacion
================================================================
 */
typedef struct{
	int  bloque;
	int  bytesOcupados;
	char nombreArchivoTemporal[TAMANIO_NOMBRE];
}payload_orden_transformacion;
typedef struct{
	char header;
	payload_orden_transformacion payload;
}paquete_orden_transformacion;

/*
================================================================
 < 4 >  Envio de ejecutable
================================================================
*/
typedef struct{
	char header;
}paquete_envio_ejecutable;

/*
================================================================
 < 5 >  Informacion de workers (reduccion local)
================================================================
 */
typedef struct{
	char ip_puerto[TAMANIO_IPPUERTO];
	int  bloque;
	int  bytesOcupados;
	char temporalDeTransformacion[TAMANIO_NOMBRE];
	char temporalDeReduccionLocal[TAMANIO_NOMBRE];
}payload_paquete_info_workers_reduxLocal;
typedef struct{
	char header;
	payload_paquete_info_workers_reduxLocal payload;
}paquete_info_workers_reduxLocal;

/*
================================================================
 < 6 >  Orden de reduccion local
================================================================
 */
typedef struct{
	char temporalDeTransformacion[TAMANIO_NOMBRE];
	char temporalDeReduccionLocal[TAMANIO_NOMBRE];
}payload_paquete_orden_reduxLocal;
typedef struct{
	char header;
	payload_paquete_orden_reduxLocal payload;
}paquete_orden_reduxLocal;

/*
================================================================
 < 7 >  Informacion de workers (reduccion global)
================================================================
 */
typedef struct{
	char ip_puerto[TAMANIO_IPPUERTO];
	char temporalDeReduccionLocal[TAMANIO_NOMBRE];
	char temporalDeReduccionGlobal[TAMANIO_NOMBRE];
	int  encargado;
}payload_paquete_info_workers_reduxGlobal;
typedef struct{
	char header;
	payload_paquete_info_workers_reduxGlobal payload;
}paquete_info_workers_reduxGlobal;

/*
================================================================
 < 8 >  Orden de reduccion global
================================================================
 */
typedef struct{
	char ip_puerto[TAMANIO_IPPUERTO];
	char temporalDeReduccionLocal[TAMANIO_NOMBRE];
	char temporalDeReduccionGlobal[TAMANIO_NOMBRE];
}payload_orden_reduxGlobal;
typedef struct{
	char header;
	payload_orden_reduxGlobal payload;
}paquete_orden_reduxGlobal;

/*
================================================================
 < 9 >  Informacion de worker (almacenado)
================================================================
 */
typedef struct{
	char ip_puerto[TAMANIO_IPPUERTO];
	char temporalDeReduccionGlobal[TAMANIO_NOMBRE];
}payload_info_workers_almacenado;
typedef struct{
	char header;
	payload_info_workers_almacenado payload;
}paquete_info_workers_almacenado;

/*
================================================================
 < a >  Orden de almacenado
================================================================
 */
typedef struct{
	char temporalDeReduccionGlobal[TAMANIO_NOMBRE];
}payload_orden_almacenado;
typedef struct{
	char header;
	payload_orden_almacenado payload;
}paquete_orden_almacenado;

/*
================================================================
 < b >  Adquisicion de bloque
================================================================
 */
typedef struct{
	int numeroDeBloque;
}payload_adquisicion_bloque;
typedef struct{
	char header;
	payload_adquisicion_bloque payload;
}paquete_adquisicion_bloque;

/*
 *
 *  PROTOCOLO DE ARCHIVOS A DEFINIR
 *
 */

/*
================================================================
 < c >  Bloque leido
================================================================
 */
typedef struct{
	char header;
}paquete_bloque_leido;

/*
================================================================
 < d >  Escritura de bloque
================================================================
 */
typedef struct{
	char header;
}paquete_escritura_bloque;

/*
================================================================
 < e >  Almacenado de archivo
================================================================
 */
typedef struct{
	char header;
}paquete_almacenado_archivo;

/*
================================================================
 < f >  Lectura de archivo
================================================================
 */
typedef struct{
	char header;
}paquete_lectura_archivo;

/*
================================================================
 < g >  Archivo leido
================================================================
 */
typedef struct{
	unsigned int tamanioDeArchivo;
}payload_almacenado_archivo;
typedef struct{
	char header;
}paquete_archivo;
// Nota: la payload debe continuar con el envio del archivo de tamaño <tamanioDeArchivo>



#endif /* PROTOCOLO_H_ */
