![Vault boy](http://pm1.narvii.com/6524/58d18d76ded01c620300ab821dd7c50524985cce_128.jpg)
# El protocolo
## Envio
```c
#include <utilidades/protocol/senders.h>
```
Para enviar un mensaje del protocolo se debera primero disponer de un socket con una conexion hacia quien queremos que reciba el mensaje.

Los mensajes son enviados por los `senders`, que tienen la siquiente forma
```c
void send_MENSAJE_A_ENVIAR(socket, argumentos, ...);
```
Los diferentes mensajes disponibles se encuentran en `utilidades/protocol/senders.h`, cada funcion `send_XX`, recibe como parametros el socket del remitente, seguido de otros parametros que varian dependiendo el mensaje.

## Recepcion
```c
#include <utilidades/protocol/receive.h>
#include <utilidades/protocol/types.h>
```
Al momento de recibir un mensaje, protocolo cuenta con la funcion `receive`, que tiene la siquiente forma:

```c
void* receive(socket, &header)
```
La funcion recibe como parametros el socket de escucha, y un puntero a una variable del tipo `T_HEADER` donde la fucnion guardara el header recibido, la funcion devuelve un puntero a un espacio de memoria que contiene la payload recibida, o NULL en caso de que se haya cortado la conexion.
> En el caso de que se hay cortado(o terminado) la conexion ademas de retornal NULL el header se setea al mensaje `FIN_COMUNICACION`

> Los headers de los mensajes disponibles se pueden encontrar en `utilidades/protocol/types.h`

> Una vez realizada la recepcion del mensaje es responsabilidad del remitente destruir la payload, para lo cual el protocolo cuenta con una funcion `destroy()`, para evitar la fuga de memoria (Trabajo en proceso)

### Ejemplo
```c
int      socket_escucha;
...							// Escuchar y bindear socket
T_HEADER cabecera;
void*    payload;

data = receive(socket_escucha,&cabecera);

//Un posible metodo para responder a los distintos mensajes es usando un switch

switch (cabecera){
  case CABECERA1:
    // Codigo de respuesta
  break;
  case CABECERA2:
  	// Codigo de respuesta
  break;
  
	...
    
  default:
  break;
}
```

> `recieve()` es una funcion que bloquea el proceso hasta que se reciba un mensaje