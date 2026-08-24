
# WebServer

Usamos ipv4 y poll

## Resources

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

##para hacer
- [] -Clases de lo necesario
- [] -Metodo post
- [] -Mejorar los tres
- [] -Mergear con lo de alex
- [] -Utilizar el sistema poll

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
- poll -> <poll.h> 
- select -> <sys/select.h>
- kqueue
- epoll -> <sys/epoll.h>


-[ ]execve
-[ ]pipe
-[ ]strerror
-[ ]gai_strerror
-[ ]errno
-[ ]dup
-[ ]dup2
-[ ]fork
-[ ]socketpair
-[ ]htons
-[ ]htonl
-[ ]ntohs
-[ ]ntohl
-[ ]select
-[ ]poll
-[ ]epoll (epoll_create
-[ ]epoll_ctl
-[ ]epoll_wait)
-[ ]kqueue (kqueue
-[ ]kevent)
-[ ]socket
-[ ]accept
-[ ]listen
-[ ]send
-[ ]recv
-[ ]shutdown
-[ ]chdir
-[ ]bind
-[ ]connect
-[ ]getaddrinfo
-[ ]freeaddrinfo
-[ ]setsockopt
-[ ]getsockname
-[ ]getprotobyname
-[ ]fcntl
-[ ]close
-[ ]read
-[ ]write
-[ ]waitpid
-[ ]kill
-[ ]signal
-[ ]access
-[ ]stat
-[ ]open
-[ ]opendir
-[ ]readdir
-[]closedir
