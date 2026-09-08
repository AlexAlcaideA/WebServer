#include "../../../include/configuration/configContext/GlobalContext.hpp"

GlobalContext::GlobalContext()
	: ConfigContext(),
	_server(NULL)
{}

GlobalContext::GlobalContext(const GlobalContext& other)
	: ConfigContext(other),
	_server(other._server ? new std::vector<ServerContext>(*other._server) : NULL)
{}

GlobalContext& GlobalContext::operator=(const GlobalContext& other)
{
	if (this != &other)
	{
		ConfigContext::operator=(other);
		delete _server;
		other._server == NULL ? _server = NULL : _server = new std::vector<ServerContext>(*other._server);
	}
	return *this;
}

GlobalContext::~GlobalContext()
{
	delete _server;
}

void GlobalContext::AddServer(const ServerContext& server)
{
	if (!_server)
		_server = new std::vector<ServerContext>();

	for (size_t i = 0; i < _server->size(); i++)
	{
		if ((*_server)[i] == server)
			throw std::invalid_argument("duplicate server block detected");
	}
	_server->push_back(server);
}

const std::vector<ServerContext>* GlobalContext::GetServers() const
{
	return _server;
}

const ServerContext* GlobalContext::GetServer(size_t index) const
{
	if (!_server || index >= _server->size())
		return NULL;
	return &((*_server)[index]);
}

const ServerContext* GlobalContext::GetServer(const std::string& ip, unsigned int port) const
{
	if (!_server)
		return NULL;
	std:: cout << "Get server ip: " << ip << " port: " << utils::unsignedLongToString(port) << std::endl;
	// Prepare keys
	ServerContext::ServerListen exactKey;
	exactKey.serverIp = ip;
	exactKey.port = port;

	ServerContext::ServerListen fallbackKey;
	fallbackKey.serverIp = "0.0.0.0";
	fallbackKey.port = port;

	// Exact IP
	for (size_t i = 0; i < _server->size(); ++i)
	{
		const ServerContext& server = (*_server)[i];
		const std::set<ServerContext::ServerListen>* listens = server.GetListens();
		if (listens)
		{
			std::set<ServerContext::ServerListen>::const_iterator it = listens->find(exactKey);
			if (it != listens->end())
				return &server;
		}
	}

	// IP 0.0.0.0
	for (size_t i = 0; i < _server->size(); ++i)
	{
		const ServerContext& server = (*_server)[i];
		const std::set<ServerContext::ServerListen>* listens = server.GetListens();
		if (listens)
		{
			std::set<ServerContext::ServerListen>::const_iterator it = listens->find(fallbackKey);
			if (it != listens->end())
				return &server;
		}
	}

	return NULL; // No server
}

ServerContext& GlobalContext::GetServer(size_t index)
{
	return (*_server)[index];
}

ServerContext* GlobalContext::GetLastServer()
{
	if (!_server || _server->empty())
		return NULL;
	return &(_server->back());
}

const ServerContext* GlobalContext::GetLastServer() const
{
	if (!_server || _server->empty())
		return NULL;
	return &(_server->back());
}

std::ostream& operator<<(std::ostream& os, const GlobalContext& other)
{
	os << "Global:\n";
	other.print(os);
	if (other.GetServers())
	{
		os << "Servers: \n";
		for (size_t i = 0; i < other.GetServers()->size(); i++)
		{
			os << "-";
			os << *(other.GetServer(i));
		}
	}
	return os;
}
