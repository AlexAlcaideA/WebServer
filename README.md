# WebServer

## Resources

### [RFC](https://es.wikipedia.org/wiki/Request_for_Comments)
RFC son las siglas de 'Request for Comments', una serie de publicaciones del grupo de trabajo de ingenieria de internet que describen diversos aspectos del funcionamiento de Internet y otras redes de computadoras, como protocolos, procedimientos, etc. y comentarios e ideas sobre estos.
#### [HTTP](https://es.wikipedia.org/wiki/Protocolo_de_transferencia_de_hipertexto)
HTTP, también conocido como protocolo de transferencia de hipertexto, es el protocolo que permite la transferencia de información a través de archivos en la world wide web. Es un protocolo sin estado, por lo cual no se guardan ninguna información sobre conexiones anteriores. Por ello se usan cookies las cuales se almacenan en el cliente y luego sriven para instituir la noción de sesión y rastrear al usuario.
##### Metodos estandar
Importantes *
- *GET
- *?HEAD
- *POST
- *?PUT
- *DELETE
- CONNECT
- OPTIONS
- TRACE PATH
##### Archivo configuracion
[Web_page-Apache](https://httpd.apache.org/docs/2.4/configuring.html)
Crear archivo de configuración llamado 'httpd.conf'. 
### [CGI](https://www.ionos.es/digitalguide/paginas-web/desarrollo-web/common-gateway-interface/)
Common Gateway Interface, es una interfaz de los se­r­vi­do­res web. (Resumen, es un método de scripting que permite dinamizar las páginas y noi tener que cargar todo de golpe y solo según se soliciten).
### Funciones
La funcion poll() y select() son estandard que pueden funcionar en distintos sistemas operativos. La funcion de epoll() es específica de linux y ofrece un mejor rendimiento. En este proyecto investigaremos poll() y epoll().
#### [POLL](https://man7.org/linux/man-pages/man2/poll.2.html)
#### [EPOLL]
- [CGI](https://en.wikipedia.org/wiki/Common_Gateway_Interface)
- [HTTP](https://www.rfc-editor.org/info/rfc9111/)
- [IETF](https://es.wikipedia.org/wiki/Grupo_de_Trabajo_de_Ingenier%C3%ADa_de_Internet)
- [Error 418](https://developer.mozilla.org/en-US/docs/Web/HTTP/Reference/Status/418)
- poll -> <poll.h> 
- select -> <sys/select.h>
- kqueue
- epoll -> <sys/epoll.h>