
# WebServer

Usamos ipv4 y poll

## Resources

- [CURL](https://curl.se/docs/httpscripting.html)
- [HTTPstatus](http.cat)
- [SNMP]()
- [NTP]()
- [Socket](https://www.linuxhowtos.org/C_C++/socket.htm)
- [Socketprogramming](https://www.geeksforgeeks.org/cpp/socket-programming-in-cpp/)
- [net socket](https://beej.us/guide/bgnet/)
- [poll](https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/poll.h.html)
- [CGI](https://en.wikipedia.org/wiki/Common_Gateway_Interface)
- [HTTP](https://www.rfc-editor.org/info/rfc9111/)
- [HTTP versions](https://developer.mozilla.org/en-US/docs/Web/HTTP/Guides/Evolution_of_HTTP)
- [IETF](https://es.wikipedia.org/wiki/Grupo_de_Trabajo_de_Ingenier%C3%ADa_de_Internet)
- [Error 418](https://developer.mozilla.org/en-US/docs/Web/HTTP/Reference/Status/418)
- [Requests](https://developer.salesforce.com/docs/atlas.en-us.apexref.meta/apexref/apex_classes_restful_http_httprequest.htm)
- [CGI tests](https://www6.uniovi.es/~antonio/ncsa_httpd/cgi/examples.html)

## Para hacer
- [] -Clases de lo necesario
- [] -Anyadir excepciones a las clases
- [x] -Metodo post
- [] -Mejorar los tres
- [] -Mergear con lo de alex
- [] -Utilizar el sistema poll

## Info
- HTTP requests:
**metodos obligatorios**
GET
POST
DELETE
**metodos opcionales**
PUT
OPTIONS
HEAD

- HTTP returns:
200 OK: Success consigui enviar el archivo
201 Created: Se ha creado un nuevo recurso
204 No Content: Success pero archivo vacio
301 Moved Permanently: Redireccion eterna
302 Found: Redireccion temporal
304 Not Modified: No ha habido cambios y el caching server puede obtener la web
400 Bad Request: Mala request o mal formulada
401 Unauthorized: Requiere autenticado
403 Forbidden: Autenticado pero no permitido
404 Not Found: Archivo no encontrado
405 Method Not Allowed: Metodo http no permitido
411 Lenght required: Un mal request no da la lenght del body
418 I'm a teapot: Servidor se niega a hacer cafe porque es una tetera
500 Internal Server Error: El servidor ha petado o ha encontrado un error
503 Service Unavailable: El servicio esta temporalmente no disponible

- Puertos:
Los puertos son 16 bit unsigned int
puerto 8080: prueba
puerto 21:FTP (transferencia de archivos)
puerto 22:ssh (conexion remota)
puerto 111:rpcbind o portmapper (sirve para llamadas y network file sistems)
puerto 631:ipp (sirve para imprimir)

### Funciones
## Librerias
- poll -> <poll.h> 
- select -> <sys/select.h>
- kqueue
- epoll -> <sys/epoll.h>

## Que hacen
-[x]execve: Sustituye el proceso por uno nuevo
-[x]pipe: Crea una pipe de comunicacion entre programas
-[x]strerror: Pone un string por el fd de error
-[x]gai_strerror: Obtiene errores de getaddrinfo
-[x]errno: Pone un string con el error del ultimo proceso
-[x]dup: Crea una copia del file descriptor
-[x]dup2: Crea una copia del file descriptor en otro file descriptor
-[x]fork: Crea un proceso hijo
-[x]socketpair: Crea dos sockets conectados entre si
-[x]htons: Ajusta short bytes de host a bytes de red
-[x]htonl: Ajusta long bytes de host a bytes de red
-[x]ntohs: Ajusta short bytes de red a bytes de host
-[x]ntohl: Ajusta long bytes de red a bytes de host
-[x]select: Espera en diversos file descriptors que ocurra algun evento
-[x]poll: Crea un objeto poll para vigilar varios sockets y 
-[x]epoll_create: Crea instancia de epoll
-[x]epoll_ctl: Anyade un socket para vigilar a epoll
-[x]epoll_wait: Espera que un socket asociado a epoll haga algo
-[x]kqueue: Vigila muchos sockets creando un kqueue
-[x]kevent: Registra eventos en kqueue y espera a que ocurran
-[x]socket: Crea un socket
-[x]accept: Extrae la primera conexion pendiente y la mete en un socket
-[x]listen: Marca un socket para escuchar conexiones
-[x]send: Envia un buffer de informacion de un socket a su par
-[x]recv: Lee informacion de un socket a un buffer
-[x]shutdown: Cierra la conexion de red en un socket
-[x]chdir: Cambia el directorio
-[x]bind: Enlaza un file descriptor a un socket
-[x]connect: Connecta un socket con otro, util para testeo y clientes
-[x]getaddrinfo: Traduce host a direcciones socket utilizables
-[x]freeaddrinfo: Libera la memoria dinamica localizada en getaddrinfo
-[x]setsockopt: Modifica las opciones del socket una vez creado
-[x]getsockname: Obtiene la info del socket en address
-[x]getprotobyname: Obtiene un identificador de protocolo
-[x]fcntl: Modifica los file descriptor, en nuestro caso hacerlos no bloqueantes.
-[x]close: Cierra un file descriptor
-[x]read: Lee un file descriptor o un buffer
-[x]write: Escribe en un file descriptor
-[x]waitpid: Espera un proceso
-[x]kill: Termina un proceso
-[x]signal: Determina handlers para senyales
-[x]access: Pregunta si tienes permisos para ejecutar un proceso
-[x]stat: Comprueba si una ruta existe
-[x]open: Abre un archivo y le asigna un file descriptor
-[x]opendir: Comprueba que es un directorio
-[x]readdir: Abre los elementos de un directorio como listado
-[x]closedir: Cierra el puntero de directorio
