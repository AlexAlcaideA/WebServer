#include "server.hpp"
#include "includes.hpp"

server::server() : _conf(NULL)
{}

// Destructor default
server::~server(void)
{
	for (size_t i = 0; i < _listenSockets.size(); ++i)
        if (_listenSockets[i] != -1)
            close(_listenSockets[i]);
	for (size_t i = 0; i < _clients.size(); ++i)
		delete _clients[i];
	if (_conf)
		delete _conf;
}
// Constructor copia
server::server(const server& other): _address(other._address), _pollFds(other._pollFds), _newPollFds(other._newPollFds), _listenSockets(other._listenSockets)
{
	if (_conf)
		delete _conf;
	_conf = new Configuration(*(other._conf));
}
// Sobrecarga operador asignacion
server &server::operator= (const server& other)
{
	if (this == &other)
		return (*this);

//Copia miembros
	this->_address = other._address;
	this->_pollFds = other._pollFds;
	this->_newPollFds = other._newPollFds;
	this->_listenSockets = other._listenSockets;
	if (_conf)
		delete _conf;
	_conf = new Configuration(*(other._conf));
	return (*this);
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

			// Add to _pollFds
			struct pollfd pfd;
            pfd.fd = fd;
            pfd.events = POLLIN;
            pfd.revents = 0;
            _pollFds.push_back(pfd);

            // Mark as used
            usedIps.insert(key.str());
		}
	}
	if (_listenSockets.empty())
		throw std::runtime_error("No listen directives found");
	std::cout << "Total listen sockets: " << _listenSockets.size() << std::endl; // TMP Eliminar al final
	for (size_t i = 0; i < _listenSockets.size(); ++i)
		std::cout << "Listen fd " << _listenSockets[i] << std::endl; // TMP Eliminar al final
	std::cout << "_pollFds size: " << _pollFds.size() << std::endl; // TMP Eliminar al final
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

	// Check if it has a return
	const LocationContext::ReturnVal* retVal = loc->GetReturnVal();
	if (retVal) // Search if it has a Return Header
	{
		map["Location"] = utils::stripQuotes(*retVal->url);
		return HttpResponse(HTTP_VER, map, retVal->code, HttpStatus::reasonPhrase(retVal->code));
	}

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
	{
		std::cout << "No encontro contexto" << std::endl; // TMP 
		return HttpResponse(HTTP_VER, map, 404, HttpStatus::reasonPhrase(404));
	}
	// Check if the method is allowed
	if (!checkLocalMethods(Http::DELETE, *loc))
        return HttpResponse(HTTP_VER, map, 405, HttpStatus::reasonPhrase(405));

	// Extract FileName
	std::string filename;
	if (! loc->GetIndexPath(req.getRequestTarget(), filename))
	{
		std::cout << "No encontro path" << std::endl; // TMP 
		return HttpResponse(HTTP_VER, map, 404, HttpStatus::reasonPhrase(404));
	}

	if (filename.empty())
		return HttpResponse(HTTP_VER, map, 400, HttpStatus::reasonPhrase(400));

	if (remove(filename.c_str()) == 0)
	{
		return HttpResponse(HTTP_VER, map, 204, HttpStatus::reasonPhrase(NO_CONTENT));
	}
    if (errno == ENOENT)
    {
		std::cout << "No encontro archivo" << std::endl; // TMP 
		return HttpResponse(HTTP_VER, map, 404, HttpStatus::reasonPhrase(404));
	}
	return HttpResponse(HTTP_VER, map, 500, HttpStatus::reasonPhrase(500));
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
	if (!cli)
		return;

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
			throw std::invalid_argument("Method not found.");
			break;
	}

	std::string answer = response.getStringMessage();
    cli->prepareResponse(answer);
}

bool server::isListenSocket(int fd) const
{
	return std::find(_listenSockets.begin(), _listenSockets.end(), fd) != _listenSockets.end();
}

client* server::findClientByFd(int fd)
{
	for (size_t i = 0; i < _clients.size(); ++i)
	{
		if (_clients[i]->getFd() == fd)
			return _clients[i];
	}
	return NULL;
}

void server::removeClientByFd(int fd)
{
    // Delete from _pollFds
    for (size_t i = 0; i < _pollFds.size(); ++i)
	{
        if (_pollFds[i].fd == fd)
		{
            _pollFds.erase(_pollFds.begin() + i);
            break;
        }
    }
    // Delete from clients
    for (size_t i = 0; i < _clients.size(); ++i)
	{
        if (_clients[i]->getFd() == fd)
		{
            delete _clients[i];
            _clients.erase(_clients.begin() + i);
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

void server::acceptNewClient(int listenFd)
{
	struct sockaddr_in clientAddr;
	socklen_t clientLen = sizeof(clientAddr);
	int clientFd = accept(listenFd, (struct sockaddr*)&clientAddr, &clientLen);
	if (clientFd == -1)
		return;

	int flags = fcntl(clientFd, F_GETFL, 0);
	if (flags == -1 || fcntl(clientFd, F_SETFL, flags | O_NONBLOCK) == -1)
	{
		close(clientFd);
		return;
	}

	client* newClient = new client(clientFd);
	std::pair<std::string, unsigned short> localAddr = getLocalAddressInfo(clientFd);
	newClient->addListener(localAddr.first, localAddr.second);
	_clients.push_back(newClient);

	struct pollfd pfd;
	pfd.fd = clientFd;
	pfd.events = POLLIN;
	pfd.revents = 0;
	_newPollFds.push_back(pfd);
}

void server::readFromClient(size_t index)
{
	int fd = _pollFds[index].fd;

	client* cli = findClientByFd(fd);
	if (!cli)
		return;

	ReceiveResult r = cli->receive();

	if (r == RECV_CLOSED)
	{
		removeClientByFd(fd);
		return;
	}
	if (r == RECV_INCOMPLETE)
		return;
	try
	{
		handleClient(fd);
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error handling with client: " << e.what() << std::endl;
		removeClientByFd(fd);
		return;
	}
	_pollFds[index].events = POLLOUT;
	_pollFds[index].revents = 0;
}

void server::writeToClient(size_t index)
{
	int fd = _pollFds[index].fd;

	client* cli = findClientByFd(fd);
	if (!cli)
		return;

	if (!cli->flushResponse())
	{
		removeClientByFd(fd);
		return;
	}

	removeClientByFd(fd);   // If keep-alive return to POLLIN
}

void server::run()
{
	while (g_running)
	{
		int ret = poll(&_pollFds[0], _pollFds.size(), TIME_OUT);
		if (ret < 0)
		{
			std::cout << "Hola error" << std::endl; // TMP
			if (g_running)
				throw std::runtime_error("poll failed");
			continue;
		}

		_newPollFds.clear();
		for (size_t i = _pollFds.size(); i-- > 0; )
		{
			int   fd      = _pollFds[i].fd;
			short revents = _pollFds[i].revents;

			if (revents == 0)
				continue;

			// HUP/ERR in client, delete
			if (!isListenSocket(fd) && (revents & (POLLHUP | POLLERR)))
			{
				removeClientByFd(fd);
				continue;
			}

			// Listener to accept
			if (isListenSocket(fd))
			{
				if (revents & POLLIN)
					acceptNewClient(fd);
				continue;
			}

			// Client: read or write
			if (revents & POLLIN)
				readFromClient(i);
			else if (revents & POLLOUT)
				writeToClient(i);
		}

		// Agregar nuevos clientes al vector principal
		for (size_t i = 0; i < _newPollFds.size(); ++i)
			_pollFds.push_back(_newPollFds[i]);
	}
}

void server::removeClient(unsigned long i)
{
	close(_pollFds[i].fd);
	delete _clients[i -1];
	_pollFds.erase(_pollFds.begin() + i);
	_clients.erase(_clients.begin() + (i-1));
}
