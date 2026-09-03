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
