#include "../../include/configContext/GlobalContext.hpp"

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
		GlobalContext::operator=(other);
		delete _server;
		other._server == NULL ? _server = NULL : _server = new std::vector<ServerContext>(*other._server);
	}
	return *this;
}

GlobalContext::~GlobalContext()
{
	delete _server;
}

std::ostream& GlobalContext::operator<<(std::ostream& os) const
{
	os << "Global:\n";
	ConfigContext::operator<<(os);
	if (_server)
	{
		os << "Servers: \n";
		for (size_t i = 0; i < _server->size(); i++)
		{
			os << "- ";
			(*_server)[i] << os;
		}	
	}
	return os;
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

std::vector<ServerContext>* GlobalContext::GetServers() const
{
	return _server;
}

ServerContext* GlobalContext::GetServer(size_t index)
{
	if (!_server || index >= _server->size())
		return NULL;
	return &((*_server)[index]);
}

ServerContext* GlobalContext::GetLastServer()
{
	if (!_server || 1 > _server->size())
		return NULL;
	return &(_server->back());
}
