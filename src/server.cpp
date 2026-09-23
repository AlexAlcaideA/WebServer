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
/*server::server(int port = 8080)
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
}*/

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
	std::cout << "Total listen sockets: " << _listenSockets.size() << std::endl; // TMP Eliminar al final
	for (size_t i = 0; i < _listenSockets.size(); ++i)
		std::cout << "Listen fd " << _listenSockets[i] << std::endl; // TMP Eliminar al final
	std::cout << "poll_fds size: " << poll_fds.size() << std::endl; // TMP Eliminar al final
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

	std::cout << "New client: " << fd << std::endl; // TMP Eliminar al final

	client* new_client = new client(fd);
	clients.push_back(new_client);

	struct pollfd pfd;
	pfd.fd = fd;
	pfd.events = POLLIN;
	pfd.revents = 0;

	poll_fds.push_back(pfd);
}

HttpResponse server::methodGet(const HttpRequest& req, const client& currentClient)
{
	std::map<std::string, std::string> map;
	const std::string* host = req.getHeader(HttpHeaders::HOST);
	std::cout << "Hola 1" << std::endl; // TMP Eliminar al final
	if (!host) // Check for Header "Host"
		return HttpResponse(HTTP_VER, map, 500, HttpStatus::reasonPhrase(500)); // TMP Cambiar por pagina y error correcto
	ServerContext::ServerListen clientListen = currentClient.GetListener();
	const ServerContext* serv = getServerByName(utils::extractHostname(*host), clientListen.serverIp, clientListen.port);
	std::cout << "Host: " << *host << " HostName: " << utils::extractHostname(*host) << std::endl; // TMP Eliminar al final
	std::cout << "Hola 2" << std::endl; // TMP Eliminar al final
	if (!serv) // Search for a server with the ip and port. Selects by name if there is more than one
	{
		std::cerr << "Server vacio." << std::endl;
		return HttpResponse(HTTP_VER, map, 404, HttpStatus::reasonPhrase(404)); // TMP Cambiar por pagina y error correcto
	}
	const LocationContext* loc = serv->GetLocation(req.getRequestTarget());
	std::cout << "Request Target: " << req.getRequestTarget() << std::endl; // TMP Eliminar al final
	std::cout << "Hola 3" << std::endl; // TMP Eliminar al final
	if (!loc) // Search for LocationContext with the same path
		return HttpResponse(HTTP_VER, map, 404, HttpStatus::reasonPhrase(404)); // TMP Cambiar por pagina y error correcto
	std::cout << "Hay location. Name: " << loc->GetPath() << std::endl; // TMP Eliminar al final
	if (!checkLocalMethods(Http::GET, *loc))
		return HttpResponse(HTTP_VER, map, 404, HttpStatus::reasonPhrase(404)); // TMP Cambiar por pagina y error correcto
	std::cout << "Method allowed" << std::endl; // TMP Eliminar al final
	const LocationContext::ReturnVal* retVal = loc->GetReturnVal();
	if (retVal) // Search if it has a Return Header
	{
		map["Location"] = utils::stripQuotes(*retVal->url);
		return HttpResponse(HTTP_VER, map, retVal->code, HttpStatus::reasonPhrase(retVal->code));
	}
	std::string rootPath = *loc->GetRoot();
	std::cout << "RootPath is: " << rootPath << std::endl; // TMP Eliminar al final
	if (!loc->GetIndexPath(req.getRequestTarget(), rootPath)) // Check for path root + index name to exist
		return HttpResponse(HTTP_VER, map, 404, HttpStatus::reasonPhrase(404)); // TMP Cambiar por pagina y error correcto
	std::string content = utils::fileToString(rootPath);
	map["Content-Type"] = "text/html";
	map["Content-Lenght"] = content.size();
	HttpResponse response(HTTP_VER, map, content.size(), content, 200, HttpStatus::reasonPhrase(200));
	std::string answer = response.getStringMessage();
	std::cout << answer << std::endl; // TMP Borrar, solo para debug de ver la respuesta

	/*std::string content = utils::fileToString("www/Pages/helloWebserver.html");
	std::map<std::string, std::string> map;
	map["Content-Type"] = "text/html";
	map["Content-Lenght"] = content.size();
	HttpResponse response(HTTP_VER, map, content.size(), content, 200, HttpStatus::reasonPhrase(200));
	std::string answer = response.getStringMessage();
	std::cout << answer << std::endl;*/

	return response;

	//return HttpResponse(HTTP_VER, map, 500, HttpStatus::reasonPhrase(500));
}

HttpResponse server::methodPost(const HttpRequest& req, const client& currentClient)
{
	std::map<std::string, std::string> map;
	// Obtain server and location
	const std::string* host = req.getHeader(HttpHeaders::HOST);
	if (!host)
		return HttpResponse(HTTP_VER, map, 400, HttpStatus::reasonPhrase(400));

	ServerContext::ServerListen clientListen = currentClient.GetListener();
	const ServerContext* serv = getServerByName(utils::extractHostname(*host), clientListen.serverIp, clientListen.port);
	if (!serv)
		return HttpResponse(HTTP_VER, map, 404, HttpStatus::reasonPhrase(404));
	const LocationContext* loc = serv->GetLocation(req.getRequestTarget());
	if (!loc)
		return HttpResponse(HTTP_VER, map, 404, HttpStatus::reasonPhrase(404));

	// Check if the method is allowed
	if (!checkLocalMethods(Http::POST, *loc))
        return HttpResponse(HTTP_VER, map, 405, HttpStatus::reasonPhrase(405));

	// Check if it has upload configuration
	const std::string* uploadStore = loc->GetUploadStore();
	if (!uploadStore || uploadStore->empty())
		return HttpResponse(HTTP_VER, map, 403, HttpStatus::reasonPhrase(403));

	// Get data from request
	const std::map<std::string, std::string>* partHeaders = req.getContentHeaders();
	const std::string* data = req.getContent();
	if (!partHeaders || !data || data->empty())
		return HttpResponse(HTTP_VER, map, 400, HttpStatus::reasonPhrase(400));
	// Extract FileName
	std::string filename;
	std::map<std::string, std::string>::const_iterator it = partHeaders->find("Content-Disposition");
	if (it != partHeaders->end())
		filename = form::extractFilename(it->second);

	if (filename.empty())
		return HttpResponse(HTTP_VER, map, 400, HttpStatus::reasonPhrase(400));

	// Sanitaize FileName
	std::string safeName = form::sanitizeFilename(filename);
	if (safeName.empty())
		return HttpResponse(HTTP_VER, map, 400, HttpStatus::reasonPhrase(400));

	// Save binary file
	std::string fullPath = *uploadStore + "/" + safeName;

	int fd = open(fullPath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
		return HttpResponse(HTTP_VER, map, 500, HttpStatus::reasonPhrase(500));

	size_t total = 0;
	while (total < data->size())
	{
		ssize_t w = write(fd, data->data() + total, data->size() - total);
		if (w <= 0)
		{
			close(fd);
			return HttpResponse(HTTP_VER, map, 500, HttpStatus::reasonPhrase(500));
		}
		total += w;
	}
	close(fd);

	// Creation Response
	return HttpResponse(HTTP_VER, map, 201, HttpStatus::reasonPhrase(201));
}

HttpResponse server::methodDelete(const HttpRequest& req, const client& currentClient)
{
	std::map<std::string, std::string> map;
	// Obtain server and location
	const std::string* host = req.getHeader(HttpHeaders::HOST);
	if (!host)
		return HttpResponse(HTTP_VER, map, 400, HttpStatus::reasonPhrase(400));
	ServerContext::ServerListen clientListen = currentClient.GetListener();
	const ServerContext* serv = getServerByName(utils::extractHostname(*host), clientListen.serverIp, clientListen.port);
	if (!serv)
		return HttpResponse(HTTP_VER, map, 404, HttpStatus::reasonPhrase(404));
	const LocationContext* loc = serv->GetLocation(req.getRequestTarget());
	if (!loc)
		return HttpResponse(HTTP_VER, map, 404, HttpStatus::reasonPhrase(404));

	// Check if the method is allowed
	if (!checkLocalMethods(Http::DELETE, *loc))
        return HttpResponse(HTTP_VER, map, 405, HttpStatus::reasonPhrase(405));

	// Check if it has upload configuration
	/*const std::string* uploadStore = loc->GetUploadStore();
	if (!uploadStore || uploadStore->empty())
		return HttpResponse(HTTP_VER, map, 403, HttpStatus::reasonPhrase(403));
*/
	// Get data from request
	/*const std::map<std::string, std::string>* partHeaders = req.getContentHeaders();
	const std::string* data = req.getContent();
	if (!partHeaders || !data || data->empty())
		return HttpResponse(HTTP_VER, map, 400, HttpStatus::reasonPhrase(400));*/
	// Extract FileName
	std::string filename;
	//std::map<std::string, std::string>::const_iterator it = partHeaders->find("Content-Disposition");
	if (it != partHeaders->end())
		filename = form::extractFilename(it->second);

	if (filename.empty())
		return HttpResponse(HTTP_VER, map, 400, HttpStatus::reasonPhrase(400));

	if (remove(filename.c_str()) == 0)
	{
		return HttpResponse(HTTP_VER, map, 203, HttpStatus::reasonPhrase(NO_CONTENT));
	}
	else
	{
		return HttpResponse(HTTP_VER, map, 404, HttpStatus::reasonPhrase(NOT_FOUND));
	}
	return HttpResponse(HTTP_VER, map, 404, HttpStatus::reasonPhrase(NOT_FOUND));
}
const ServerContext* server::getServerByName(const std::string& name, const std::string& ip, unsigned int port) const
{
	const std::vector<ServerContext>* servers = _conf->GetConf().GetServers();
	if (!servers)
		return NULL;

	// Primera pasada: buscar coincidencia exacta de IP y puerto
	const ServerContext* exactDefault = NULL;
	for (size_t i = 0; i < servers->size(); ++i)
	{
		const ServerContext& srv = (*servers)[i];
		if (!srv.hasListen(ip, port, true)) // true = solo IP exacta
			continue;
		if (!exactDefault)
			exactDefault = &srv;
		if (srv.checkServerNames(name))
			return &srv;
	}

	// Segunda pasada: buscar con 0.0.0.0 (si no hubo exacta)
	if (!exactDefault)
	{
		for (size_t i = 0; i < servers->size(); ++i)
		{
			const ServerContext& srv = (*servers)[i];
			if (!srv.hasListen(ip, port, false)) // false = acepta 0.0.0.0
				continue;
			if (srv.checkServerNames(name))
				return &srv;
			if (!exactDefault) exactDefault = &srv; // primer fallback
		}
	}

	// Si no hay coincidencia de nombre, devolver el default encontrado
	return exactDefault;
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

    HttpRequest request(cli->getRawData());
    std::cout << "Request text:\n" << request << std::endl;

    HttpResponse response;
	switch (request.getMethod())
	{
		case Http::GET:
			response = methodGet(request, *cli);
			break;
		case Http::POST:
			response = methodPost(request, *cli);
			break;
		case Http::DELETE:
			response = methodDelete(request, *cli);
			break;
		default:
			break;
	}

    std::string answer = response.getStringMessage();
	std::cout << "Response:\n" << answer << std::endl;
    // Enviar todo el string (manejar envío parcial)
    size_t total = 0;
    while (total < answer.size())
	{
        ssize_t sent = send(fd, answer.c_str() + total, answer.size() - total, MSG_NOSIGNAL);
        if (sent <= 0)
		{
            removeClientByFd(fd);
            return;
        }
        total += sent;
    }

    // Eliminar cliente despues de enviar (si es HTTP/1.1 con Connection: close)
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
    // Delete from poll_fds
    for (size_t i = 0; i < poll_fds.size(); ++i)
	{
        if (poll_fds[i].fd == fd)
		{
            poll_fds.erase(poll_fds.begin() + i);
            break;
        }
    }
    // Delete from clients
    for (size_t i = 0; i < clients.size(); ++i)
	{
        if (clients[i]->getFd() == fd)
		{
            delete clients[i];
            clients.erase(clients.begin() + i);
            break;
        }
    }
    close(fd);
}

std::pair<std::string, unsigned short> server::getLocalAddressInfo(int clientFd)
{
	struct sockaddr_in localAddr;
	socklen_t localLen = sizeof(localAddr);
	if (getsockname(clientFd, (struct sockaddr*)&localAddr, &localLen) == 0)
	{
		std::string ip = utils::ipToString(localAddr.sin_addr.s_addr);
		unsigned short port = ntohs(localAddr.sin_port);
		return std::make_pair(ip, port);
	}
	// Manejo de error
	return std::make_pair("", 0);
}

bool server::checkLocalMethods(Http::Method method, const LocationContext& local)
{
	if (!local.GetLimitExcepts())
		return false;
	for (size_t i = 0; i < local.GetLimitExcepts()->size(); i++)
		if (method == *(local.GetLimitExcept(i)))
			return true;
	return false;
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
					struct sockaddr_in clientAddr;
					socklen_t clientLen = sizeof(clientAddr);
					int clientFd = accept(fd, (struct sockaddr*)&clientAddr, &clientLen);
					if (clientFd != -1)
					{
						std::cout << "Llega nuevo cliente!" << std::endl; // TMP Eliminar al final
						// Configurar non-blocking
						int flags = fcntl(clientFd, F_GETFL, 0);
						if (flags == -1)
							throw std::runtime_error("fcntl F_GETFL failed");
						if (fcntl(clientFd, F_SETFL, flags | O_NONBLOCK) == -1)
							throw std::runtime_error("fcntl F_SETFL failed");
						std::cout << "New client: " << clientFd << std::endl; // TMP Eliminar al final

						client* newClient = new client(clientFd);

						std::pair<std::string, unsigned short> localAddr = getLocalAddressInfo(clientFd);

						newClient->AddListener(localAddr.first, localAddr.second);
						// Mostrar información de depuración
						std::cout << "Server local address: " << localAddr.first << ":" << localAddr.second << std::endl; // TMP Eliminar al final

						// También puedes mostrar la dirección remota si la necesitas
						std::cout << "Client remote address: " << utils::ipToString(clientAddr.sin_addr.s_addr)
							<< ":" << ntohs(clientAddr.sin_port) << std::endl; // TMP Eliminar al final

						clients.push_back(newClient);

						struct pollfd pfd;
						pfd.fd = clientFd;
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
					if (cli)
					{
						if (poll_fds[i].revents & (POLLHUP | POLLERR))
							removeClientByFd(fd);
						else if (poll_fds[i].revents & POLLIN)
							handleClient(fd);
    				}
				}
			}
			else if (poll_fds[i].revents & (POLLHUP | POLLERR))
			{
				std::cerr << "Error cliente" << std::endl; // TMP Eliminar al final
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
