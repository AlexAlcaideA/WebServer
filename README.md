*This project has been created as part of the 42 curriculum by aalcaide, epascual.*

# WebServer

## Description

WebServer is an HTTP/1.1 server written in C++98 from scratch. Its purpose is to implement the fundamentals of a web server: accepting client connections, parsing HTTP requests, serving static files, handling file uploads, executing CGI scripts, and managing multiple virtual servers on different ports — all using a single non-blocking `poll()` loop.

The server is designed to be resilient, non-blocking at all times, and compatible with standard web browsers. It supports GET, POST, and DELETE methods, custom error pages, directory listing, HTTP redirections, and CGI execution based on file extension.

## Instructions

### Compilation

```bash
make			# builds the executable `webserv`
make clean		# removes object files
make fclean		# removes object files and the executable
make re			# rebuilds from scratch
```

### Installation

You must create your own configuration file and make sure the paths in it match the actual paths in the project directory.

### Execution

```bash
./webserv		# It uses a default configuration path in config/default-config.conf. If not found it closes.
./webserv <path>	# It uses a path to a configuration file.
```
-Example
```bash
./webserv config/default-config.conf
```

### Configuration

The configuration file follows an NGINX-like syntax. It supports:

- Multiple server blocks listening on different interface:port pairs.

- root, index, autoindex, error_page, client_max_body_size.

- location blocks with limit_except, return, upload_store, cgi_handler.

- server_name for virtual hosts: several server blocks can share the same interface:port and be distinguished by the HTTP Host header.

See the config/ directory for ready-to-use examples.

## Resources

### RFC
RFC stands for "Request for Comments": a series of publications from the Internet Engineering Task Force describing various aspects of how the Internet and other networks work, such as protocols, procedures, comments, and ideas about them.

- [RFC](https://es.wikipedia.org/wiki/Request_for_Comments)

- [RFC 9111-HTTP Caching](https://www.rfc-editor.org/info/rfc9111/)

- [RFC 3253-Versioning Extensions to WebDAV](https://datatracker.ietf.org/doc/html/rfc3253)

### HTTP
HTTP (HyperText Transfer Protocol) is the protocol that enables the transfer of information over files on the World Wide Web. It is a stateless protocol: no information is kept about previous connections. To work around this, cookies are used — stored on the client side — to establish the notion of a session and to track users.

- [HTTP](https://es.wikipedia.org/wiki/Protocolo_de_transferencia_de_hipertexto)

- [List of HTTP request methods — Mockoon](https://mockoon.com/articles/list-http-request-methods/)

- [HTTP.CAT](https://http.cat/)

- [Error 418](https://developer.mozilla.org/en-US/docs/Web/HTTP/Reference/Status/418)

#### Standard methods

Required: GET, POST, DELETE.

Optional: HEAD, PUT, CONNECT, OPTIONS, TRACE.

#### Configuration file
[Apache HTTP Server — Configuration files](https://httpd.apache.org/docs/2.4/configuring.html)

### CGI
Common Gateway Interface (CGI) is an interface for web servers. In short, it is a scripting method that allows pages to be dynamic, so the whole page does not have to be loaded at once-content is generated only when requested.

[CGI](https://www.ionos.es/digitalguide/paginas-web/desarrollo-web/common-gateway-interface/)

### Event notification functions

poll() and select() are standard and work across different operating systems. epoll() is Linux-specific and offers better performance. In this project we investigated poll() and epoll().

- [poll(2)](https://man7.org/linux/man-pages/man2/poll.2.html)

- [poll.h](https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/poll.h.html)

### Sockets and network programming

- [Socket](https://www.linuxhowtos.org/C_C++/socket.htm)

- [Socketprogramming](https://www.geeksforgeeks.org/cpp/socket-programming-in-cpp/)

- [net socket](https://beej.us/guide/bgnet/)

### Testing tools

- [curl](https://curl.se/docs/httpscripting.html)

- [siege](https://www.joedog.org/siege/manual/)

- [Valgrind](https://valgrind.org/docs/manual/index.html)

### AI usage

AI was used as a learning assistant during the development of this project, specifically for:

- Understanding HTTP internals: clarifying the structure of HTTP requests/responses, headers, chunked transfer encoding, and the semantics of status codes.

- Reviewing the event loop design: discussing how poll() should be integrated with the client lifecycle and CGI pipes, and comparing our approach against NGINX's behaviour.

- Debugging memory and fd leaks: interpreting Valgrind output and identifying which classes were responsible for cleanup.

- Writing tests: generating curl and siege commands to validate GET, POST, DELETE, CGI, body size limits, and stress behaviour.

AI was not used to generate the core implementation. All code was reviewed, understood, and adapted by the team. Every AI-generated suggestion was tested and validated against the subject requirements and against NGINX when behaviour was ambiguous.

## Reference notes

### HTTP response status codes

- 200 OK: the file was successfully sent.

- 201 Created: a new resource was created.

- 202 Accepted: 

- 204 No Content: success, but the response body is empty.

- 301 Moved Permanently: permanent redirection.

- 302 Found: temporary redirection.

- 304 Not Modified: nothing changed; the caching client can reuse the cached version.

- 400 Bad Request: malformed request.

- 401 Unauthorized: authentication required.

- 403 Forbidden: authenticated but not allowed.

- 404 Not Found: file not found.

- 405 Method Not Allowed: HTTP method not allowed on this route.

- 411 Length Required: the request does not specify the body length.

- 418 I'm a teapot: the server refuses to brew coffee because it is a teapot.

- 500 Internal Server Error: the server crashed or encountered an error.

- 501 Not Implemented: the server does not implement this method.

- 503 Service Unavailable: the service is temporarily unavailable.

- 504 Gateway Timeout:

### Ports

Ports are 16-bit unsigned integers.

- 8080: testing port.

- 21: FTP (file transfer).

- 22: SSH (remote connection).

- 111: rpcbind / portmapper (used for RPC and network file systems).

- 631: IPP (printing).

### Functions

#### Libraries

- poll -> <poll.h>

- select -> <sys/select.h>

- kqueue -> <sys/time.h> <sys/event.h> <sys/types.h>

- epoll -> <sys/epoll.h>

- execve, pipe -> <unistd.h>

- strerror -> <string.h>

- errno -> \<cerrno>

- gai_strerror -> <netdb.h> <sys/socket.h>

#### What each function does
- [x] execve: replaces the current process with a new one.
- [x] pipe: creates a communication pipe between programs.
- [x] strerror: returns the error string for a given error code.
- [x] gai_strerror: returns the error string for getaddrinfo.
- [x] errno: holds the error code of the last operation.
- [x] dup: creates a copy of a file descriptor.
- [x] dup2: duplicates a file descriptor into another file descriptor.
- [x] fork: creates a child process.
- [x] socketpair: creates two connected sockets.
- [x] htons: converts a short from host to network byte order.
- [x] htonl: converts a long from host to network byte order.
- [x] ntohs: converts a short from network to host byte order.
- [x] ntohl: converts a long from network to host byte order.
- [x] select: waits for events on several file descriptors.
- [x] poll: creates a poll object to monitor several sockets.
- [x] epoll_create: creates an epoll instance.
- [x] epoll_ctl: adds a socket to be monitored by epoll.
- [x] epoll_wait: waits for a socket associated with epoll to become ready.
- [x] kqueue: monitors many sockets by creating a kqueue.
- [x] kevent: registers events in a kqueue and waits for them.
- [x] socket: creates a socket.
- [x] accept: extracts the first pending connection into a new socket.
- [x] listen: marks a socket as listening for connections.
- [x] send: sends a buffer of information through a socket.
- [x] recv: reads information from a socket into a buffer.
- [x] shutdown: closes the network connection on a socket.
- [x] chdir: changes the working directory.
- [x] bind: binds a file descriptor to a socket.
- [x] connect: connects a socket to another, useful for testing and clients.
- [x] getaddrinfo: translates a host into usable socket addresses.
- [x] freeaddrinfo: frees the dynamic memory allocated by getaddrinfo.
- [x] setsockopt: modifies socket options after creation.
- [x] getsockname: retrieves the socket's address information.
- [x] getprotobyname: retrieves a protocol identifier.
- [x] fcntl: modifies file descriptors — in our case, to make them non-blocking.
- [x] close: closes a file descriptor.
- [x] read: reads from a file descriptor into a buffer.
- [x] write: writes to a file descriptor.
- [x] waitpid: waits for a process.
- [x] kill: terminates a process.
- [x] signal: sets handlers for signals.
- [x] access: checks permissions for a file.
- [x] stat: checks whether a path exists.
- [x] open: opens a file and assigns it a file descriptor.
- [x] opendir: opens a directory.
- [x] readdir: reads directory entries one by one.
- [x] closedir: closes a directory pointer.