#include "server.hpp"
#include "includes.hpp"

// Destructor default
server::~server(void)
{
	close(this->server_fd);
	for (size_t i = 0; i < _listenSockets.size(); ++i)
        if (_listenSockets[i] != -1)
            close(_listenSockets[i]);
	for (size_t i = 0; i < clients.size(); ++i)
		delete clients[i];
	if (_conf)
		delete _conf;
}
// Constructor copia
server::server(const server& otro):server_fd(otro.server_fd), address(otro.address)
{
}
// Sobrecarga operador asignacion
server &server::operator= (const server& otro)
{
	if (this == &otro)
	{
		return (*this);
	}
//Copia miembros
	this->server_fd = otro.server_fd;
	this->address = otro.address;
	return (*this);
}

int	server::get_server_fd(void) const
{
	return (server_fd);
}
//Set socket para constructor
void	server::setupSocket()
{
	int opt = 1;

	struct protoent *protocolo = getprotobyname("tcp");
	this->server_fd = socket(AF_INET, SOCK_STREAM, protocolo->p_proto);
	if (server_fd == -1)
		throw std::runtime_error("socket failed");

	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR,
		&opt, sizeof(opt));

	if (bind(server_fd,
			(struct sockaddr *)&address,
			sizeof(address)) == -1)
	{
		close(server_fd);
		throw std::runtime_error("bind failed");
	}

	if (listen(server_fd, SOMAXCONN) == -1)
	{
		close(server_fd);
		throw std::runtime_error("listen failed");
	}
}
// Constructor parametrizado
server::server(int port = 8080)
	: server_fd(-1), port(port), _conf(NULL)
{
	std::memset(&address, 0, sizeof(address));

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(port);

	setupSocket();
	struct pollfd pfd;
	pfd.fd = server_fd;
	pfd.events = POLLIN;
	pfd.revents = 0;
	poll_fds.push_back(pfd);
	std::cout << "Server listening on port " << port << std::endl;
}

bool setAddress(const std::string& ip, unsigned short port, sockaddr_in& addr)
{
	struct addrinfo hints, *res;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_NUMERICHOST;

	std::ostringstream port_str;
	port_str << port;

	int status = getaddrinfo(ip.c_str(), port_str.str().c_str(), &hints, &res);
	if (status != 0)
		return false;

	struct sockaddr_in* ipv4 = reinterpret_cast<struct sockaddr_in*>(res->ai_addr);
	addr = *ipv4;
	freeaddrinfo(res);
	return true;
}

server::server(const Configuration& conf)
	: _conf(new Configuration(conf))
{
	_address.clear();

	std::set<std::string> usedIps;
	GlobalContext global = _conf->GetConf();

	for (size_t i = 0; i < global.GetServers()->size(); i++)
	{
		ServerContext serverCont = global.GetServer(i);

		if (!serverCont.GetListens())
			continue;

		for (size_t j = 0; j < serverCont.GetListens()->size(); j++)
		{
			ServerContext::ServerListen servListen = *serverCont.GetListen(j);

			// Key to avoid duplicates
			std::ostringstream key;
            key << servListen.serverIp << ":" << servListen.port;
            if (usedIps.find(key.str()) != usedIps.end())
                continue; // Already configured

			struct sockaddr_in addr;
			if (!setAddress(servListen.serverIp, servListen.port, addr))
				throw std::runtime_error("Invalid listen address: " + key.str());
			
			// Create TCP socket
			int fd = socket(AF_INET, SOCK_STREAM, 0);
			if (fd == -1)
				throw std::runtime_error("socket failed for " + key.str());

			// Option SO_REUSEADDR
			int opt = 1;
			if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
			{
				close(fd);
				throw std::runtime_error("setsockopt failed for " + key.str());
			}

			// Bind
            if (bind(fd, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) == -1)
            {
                close(fd);
                throw std::runtime_error("bind failed for " + key.str());
            }

			// Listen
            if (listen(fd, SOMAXCONN) == -1)
            {
                close(fd);
                throw std::runtime_error("listen failed for " + key.str());
            }

			// Save fd in client
			_listenSockets.push_back(fd);

			// Add to poll_fds
			struct pollfd pfd;
            pfd.fd = fd;
            pfd.events = POLLIN;
            pfd.revents = 0;
            poll_fds.push_back(pfd);

            // Mark as used
            usedIps.insert(key.str());
		}
	}
	if (_listenSockets.empty())
		throw std::runtime_error("No listen directives found");
}

void server::acceptClient()
{
	int fd;

	fd = accept(server_fd, NULL, NULL);
	if (fd == -1)
	{
		std::cerr << "accept: " << strerror(errno) << std::endl;
		return;
	}
	fcntl(fd, F_SETFL, O_NONBLOCK);

	std::cout << "New client: " << fd << std::endl;

	client* new_client = new client(fd);
	clients.push_back(new_client);

	struct pollfd pfd;
	pfd.fd = fd;
	pfd.events = POLLIN;
	pfd.revents = 0;

	poll_fds.push_back(pfd);
}

HttpResponse server::methodGet(const HttpRequest& req)
{
	if (req.getRequestTarget() == "/")
	{
		std::string content = utils::fileToString("www/Pages/helloWebserver.html");
		std::map<std::string, std::string> map;
		map["Content-Type"] = "text/html";
		map["Content-Lenght"] = content.size();
		HttpResponse response("HTTP/1.1", map, content.size(), content, 200, HttpStatus::reasonPhrase(200));
		std::string answer = response.getStringMessage();
		std::cout << answer << std::endl;

		return response;
	}
	std::map<std::string, std::string> map;
	return HttpResponse("HTTP/1.1", map, 500, HttpStatus::reasonPhrase(500));
}

HttpResponse methodPost(const HttpRequest& req)
{
	(void)req;
	std::map<std::string, std::string> map;
	return HttpResponse("HTTP/1.1", map, 500, HttpStatus::reasonPhrase(500));
}

void server::handleClient(int fd)
{
	
	client* cli = findClientByFd(fd);
    if (!cli) return;

    if (!cli->receive())
	{
        removeClientByFd(fd);
        return;
    }

    // Asegurar terminador nulo en el buffer (si no lo hace receive)
    // ...

    HttpRequest request(*cli->getBuffer());
    std::cout << request << std::endl;

    HttpResponse response;
    switch (request.getMethod())
	{
        case Http::GET:
			response = methodGet(request);
			break;
        case Http::POST:
			response = methodPost(request);
			break;
        case Http::DELETE:
			/* ... */
			break;
        default:
			break;
    }

    std::string answer = response.getStringMessage();
    // Enviar todo el string (manejar envío parcial)
    size_t total = 0;
    while (total < answer.size())
	{
        ssize_t sent = send(fd, answer.data() + total, answer.size() - total, MSG_NOSIGNAL);
        if (sent <= 0)
		{
            removeClientByFd(fd);
            return;
        }
        total += sent;
    }

    // Eliminar cliente después de enviar (si es HTTP/1.1 con Connection: close)
    removeClientByFd(fd);
}

/*bool server::isListenSocket(int fd) const
{
	return std::find(clients.begin(), clients.end(), fd) != clients.end();
}*/

bool server::isListenSocket(int fd) const
{
	return std::find(_listenSockets.begin(), _listenSockets.end(), fd) != _listenSockets.end();
}

client* server::findClientByFd(int fd)
{
	for (size_t i = 0; i < clients.size(); ++i)
	{
		if (clients[i]->getFd() == fd)
			return clients[i];
	}
	return NULL;
}

void server::removeClientByFd(int fd)
{
    // Eliminar de poll_fds
    for (size_t i = 0; i < poll_fds.size(); ++i)
	{
        if (poll_fds[i].fd == fd) {
            poll_fds.erase(poll_fds.begin() + i);
            break;
        }
    }
    // Eliminar de clients
    for (size_t i = 0; i < clients.size(); ++i)
	{
        if (clients[i]->getFd() == fd) {
            delete clients[i];
            clients.erase(clients.begin() + i);
            break;
        }
    }
    close(fd);
}

void server::run()
{
	/*while (g_running)
	{
		if (poll(&poll_fds[0], poll_fds.size(), -1) == -1)
			throw std::runtime_error("poll failed");

		for (size_t i = 0; i < poll_fds.size(); i++)
		{
			if (poll_fds[i].revents == 0)
				continue;

			if (poll_fds[i].fd == server_fd)
			{
				acceptClient();
			}
			else
			{
				handleClient(i);
			}
		}
	}*/

	while (g_running)
	{
		
		int ret = poll(&poll_fds[0], poll_fds.size(), TIME_OUT);
		if (ret < 0)
		{
			std::cout << "Hola error" << std::endl;
			if (errno == EINTR)
				continue;
			throw std::runtime_error("poll failed");
		}

		std::vector<struct pollfd> new_poll_fds;  // temporales

		for (size_t i = 0; i < poll_fds.size(); ++i)
		{
			if (poll_fds[i].revents & POLLIN)
			{
				int fd = poll_fds[i].fd;
				if (isListenSocket(fd))
				{
					std::cout << "Listen socket" << std::endl;
					struct sockaddr_in client_addr;
					socklen_t client_len = sizeof(client_addr);
					int client_fd = accept(fd, (struct sockaddr*)&client_addr, &client_len);
					if (client_fd != -1)
					{
						std::cout << "Llega nuevo cliente!" << std::endl;
						// Configurar non-blocking
						int flags = fcntl(client_fd, F_GETFL, 0);
						if (flags == -1)
							throw std::runtime_error("fcntl F_GETFL failed");
						if (fcntl(client_fd, F_SETFL, flags | O_NONBLOCK) == -1)
							throw std::runtime_error("fcntl F_SETFL failed");

						std::cout << "New client: " << client_fd << std::endl;

						client* new_client = new client(client_fd);
						clients.push_back(new_client);

						struct pollfd pfd;
						pfd.fd = client_fd;
						pfd.events = POLLIN | POLLHUP | POLLERR;
						pfd.revents = 0;
						new_poll_fds.push_back(pfd);
					}
				}
				else
				{
					// Es un cliente
					int fd = poll_fds[i].fd;
					// Buscar el cliente por fd
					client* cli = findClientByFd(fd);
					if (cli) {
						if (poll_fds[i].revents & (POLLHUP | POLLERR))
							removeClientByFd(fd);
						else if (poll_fds[i].revents & POLLIN)
							handleClient(fd);
    				}
				}
			}
			else if (poll_fds[i].revents & (POLLHUP | POLLERR))
			{
				std::cerr << "Error cliente" << std::endl;
				// Cliente desconectado o error: cerrar y eliminar
				// ...
			}
    	}
		// Agregar nuevos clientes al vector principal
		for (size_t i = 0; i < new_poll_fds.size(); ++i)
			poll_fds.push_back(new_poll_fds[i]);
	}

}
void server::removeClient(unsigned long i)
{
	close(poll_fds[i].fd);
	delete clients[i -1];
	poll_fds.erase(poll_fds.begin() + i);
	clients.erase(clients.begin() + (i-1));
}
