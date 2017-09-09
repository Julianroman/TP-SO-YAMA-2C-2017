# Protocolo
## Mensajes

### | '0' | Solicitud de procesamiento
Master -> YAMA : indica sobre que archivo desea operar

**Formato**

    | '0'  |nombreArchivo     |
    | char |char[TAMANIO_RUTA]|

**Respuesta**
+ Caso: no se pudo acceder al archivo pedido
  * Mensaje de estado indicando error


+ Caso: exito
  1. N mensajes de informacion de workers
  2. Mensaje de estado indicando fin de lista

### | '1' | Mensaje de estado
XXXX -> XXXX : Mensaje de Status

**Formato**

    | '1'  |mensaje          |
    | char |char[TAMANIO_MSG]|

### | '2' | Informacion de workers (transformacion)
YAMA -> Master : informacion de los workers sobre los que se debera operar

**Formato**

    | '2'  |ip:puerto             |bloque|bytesOcupados|nombreArchivoTemporal|
    | char |char[TAMANIO_IPPUERTO]|int   |int          |char[TAMANIO_NOMBRE] |

** Comportamiento : Master **

  + Espera un mensaje de error o n mensajes de informacion seguidos de un mensaje de estado con un fin de lista

### | '3' | Orden de transformacion
Master -> Workers : orden de ejecucion de la transformacion

**Formato**

    | '3'  |bloque|bytesOcupados|nombreArchivoTemporal|
    | char |int   |int          |char[TAMANIO_NOMBRE] |

**Comportamiento : Worker**
  + Queda a la espera de n ordenes de ejecucion seguidas de un mensaje de envio de ejecutable

### | '4' | Envio de ejecutable
Master -> Worker : tamaño del archivo de ejecucion

**Formato**

    | '4'  |tamanioArchivo|
    | char |unsigned int  |

**Comportamiento : Worker**
  + El worker tiene la responsabilidad de recibir un paquete ,del tamaño obtenido en el mensaje ,que estara conformado por los contenidos del archivo

  **Comportamiento : Master**
  + El worker tiene la responsabilidad de enviar un paquete ,del tamaño enviado previamente en el mensaje, que estara conformado por los contenidos del archivo

### | '5' | Informacion de workers (reduccion local)
YAMA -> Master : informacion de los workers sobre los que se debera operar

**Formato**

    | '5'  |ip:puerto             |temporalDeTransformacion|temporalDeReduccionLocal|
    | '5'  |char[TAMANIO_IPPUERTO]|char[TAMANIO_NOMBRE]    |char[TAMANIO_NOMBRE]    |

**Comportamiento : Master**
  + Espera un mensaje de error o n mensajes de informacion seguidos de un mensaje de estado con un fin de lista


### | '6' | Orden de reduccion local
Master -> Worker : orden de ejecucion de la reduccion local

**Formato**

    | '6'  |temporalDeTransformacion|temporalDeReduccionLocal|
    | '6'  |char[TAMANIO_NOMBRE]    |char[TAMANIO_NOMBRE]    |

**Comportamiento : Worker**
  + Queda a la espera de n ordenes de reduccion local seguidas de un mensaje de envio de ejecutable

### | '7' | Informacion de workers (reduccion global)
YAMA -> Master : informacion de los workers sobre los que se debera operar

**Formato**

    | '7'  |ip:puerto             |temporalReduccionLocal|temporalReduccionGlobal|Encargado|
    | char |char[TAMANIO_IPPUERTO]|char[TAMANIO_NOMBRE]  |char[TAMANIO_NOMBRE]   |int      |

**Comportamiento : Master**
  + Espera un mensaje de error o N mensajes de informacion seguidos de un mensaje de estado con un fin de lista


### | '8' | Orden de reduccion global
Master -> Worker : orden de ejecucion de la reduccion global enviada al worker encargado

**Formato**

    | '8'  |ip:puerto             |temporalDeReduccionLocal|temporalDeReduccionGlobal|
    | char |char[TAMANIO_IPPUERTO]|char[TAMANIO_NOMBRE]    |char[TAMANIO_NOMBRE]     |

**Comportamiento : Worker**
  + Queda a la espera de n ordenes de reduccion global seguidas de un mensaje de estado con un fin de lista

### | '9' | Informacion de worker (almacenado)
YAMA -> Master : informacion del worker encargado de almacenar sus resultados

**Formato**

    | '9'  |ip:puerto             |temporalDeReduccionGlobal|
    | char |char[TAMANIO_IPPUERTO]|char[TAMANIO_NOMBRE]     |

### | 'a' | Orden de almacenado
Master -> Worker : orden de almacenamiento enviada al worker encargado

**Formato**

    | 'a'  |temporalDeReduccionGlobal|
    | char |char[TAMANIO_NOMBRE]     |

**Comportamiento : Worker**
  + Queda a la espera de n ordenes de reduccion global seguidas de un mensaje de estado con un fin de lista

### | 'b' | Adquisicion de bloque
FileSystem -> DataNode : Devolverá el contenido del bloque solicitado almacenado en el Espacio de Datos

**Formato**

    | 'b'  |numeroDeBloque|
    | char |unsigned int  |

### | 'c' | Bloque leido
DataNode -> FileSystem : Bloque de archivo

**Formato**

    | 'c'  |contenidoDelBloque|
    | char |A DEFINIR         |

### | 'd' | Escritura de bloque
FileSystem -> DataNode : Grabará los datos enviados en el bloque solicitado del Espacio de Datos

**Formato**

    | 'd'  |numeroDeBloque|contenidoDelBloque|
    | char |unsigned int  |A DEFINIR         |

### | 'e' | Almacenado de archivo
{YAMA,Worker,Consola} -> FileSystem : Almacena el archivo

**Formato**

    | 'e'  |rutaCompleta      |nombreArchivo       |tipo(texto o binario)|tamanioArchivo|
    | 'e'  |char[TAMANIO_RUTA]|char[TAMANIO_NOMBRE]|char                 |unsigned int  |

**Respuesta**

  + Mensaje de estado con exito o informacion de error
**Comportamiento : FileSystem**
    + El filesystem tiene la responsabilidad de recibir un paquete ,del tamaño obtenido en el mensaje ,que estara conformado por los contenidos del archivo

**Comportamiento : Remitente**
    + El remitente tiene la responsabilidad de enviar un paquete ,del tamaño enviado previamente en el mensaje, que estara conformado por los contenidos del archivo

### | 'f' | Lectura de archivo
Consola -> FileSystem : Lee un archivo

**Formato**

    | 'f'  |rutaCompleta      |nombreArchivo       |
    | char |char[TAMANIO_RUTA]|char[TAMANIO_NOMBRE]|

**Respuesta**

  + Caso: no se pudo acceder al archivo pedido
    + Mensaje de estado indicando error
  + Caso: exito
     + Mensaje de archivo leido con el contenido del archivo

### | 'g' | Archivo leido
FileSystem -> Consola : Contenido de un archivo

**Formato**

    | 'g'  |contenidoDelArchivo                    |
    | char |tamaño variable definido por premensaje|
