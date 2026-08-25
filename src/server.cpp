#include "server.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
// Destructor default
server::~server(void)
{
	close(this->server_fd);
}
// Constructor parametrizado
server::server(int puerto = 8080)
{
    this->server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (this->server_fd == -1)
    {
        perror("socket");
        _exit(1);
    }
    sockaddr_in address;
    std::memset(this.address, 0, sizeof(address));
    this->address.sin_family = AF_INET;
    this->address.sin_addr.s_addr = INADDR_ANY;
    this->address.sin_port = htons(puerto);
    if (bind(this->server_fd, (sockaddr*)(this.address), sizeof(this->address)) == -1)
    {
        perror("bind");
        close(this->server_fd);
        _exit(1);
    }
	    if (listen(server_fd, 10) == -1)
    {
        perror("listen");
        close(server_fd);
        _exit(1);
    }
    std::cout << "Server listening on port " << puerto << std::endl;
}
// Constructor copia
server::server(const server& otro)
{
*this = otro;
}
// Sobrecarga operador asignacion
server &server::operator= (const server& otro)
{
if (this == &otro)
{
	return (*this);
}
//Copia miembros
//this->a = otro.a;
	return (*this);
}

