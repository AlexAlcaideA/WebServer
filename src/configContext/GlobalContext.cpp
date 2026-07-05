#include "../../include/configContext/GlobalContext.hpp"

GlobalContext::GlobalContext()
	: ConfigContext(),
	_server(NULL)
{}

GlobalContext::GlobalContext(const GlobalContext& other)
{
	*this = other;
}

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
