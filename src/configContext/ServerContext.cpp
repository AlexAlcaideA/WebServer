#include "../../include/configContext/ServerContext.hpp"

ServerContext::ServerContext()
	: ConfigContext(),
	_listen(NULL), _serverName(NULL), _location(NULL), _cgiHandlers(NULL)
{}

ServerContext::ServerContext(const ServerContext& other)
{
	*this = other;
}

ServerContext& ServerContext::operator=(const ServerContext& other)
{
	if (this != &other)
	{
		ConfigContext::operator=(other);
		delete _listen;
		other._listen == NULL ? _listen = NULL : _listen = new std::set<ServerListen>(*other._listen);
		delete _serverName;
		other._serverName == NULL ? _serverName = NULL : _serverName = new std::set<std::string>(*other._serverName);
		delete _location;
		other._location == NULL ? _location = NULL : _location = new std::set<LocationContext>(*other._location);
		delete _cgiHandlers;
		other._cgiHandlers == NULL ? _cgiHandlers = NULL : _cgiHandlers = new std::map<std::string, std::string>(*other._cgiHandlers);
	}
	return *this;
}

ServerContext::~ServerContext()
{
	delete _listen;
	delete _serverName;
	delete _location;
	delete _cgiHandlers;
}

bool ServerContext::operator==(const ServerContext& other) const
{
	if (_listen && other._listen)
	{
		if (*_listen != *other._listen)
			return false;
	}
	else if (_listen != other._listen)
		return false;

	if (_serverName && other._serverName)
	{
		if (*_serverName != *other._serverName)
			return false;
	}
	else if (_serverName != other._serverName)
		return false;
	return true;
}

void ServerContext::SetListen(const unsigned int& port)
{
	SetListen("0.0.0.0", port);
}

void ServerContext::SetListen(const std::string& ip, const unsigned int& port)
{
	if (!_listen)
		_listen = new std::set<ServerListen>();

	ServerListen sl;
	sl.serverIp = ip;
	sl.port = port;

	std::pair<std::set<ServerListen>::iterator, bool> result = _listen->insert(sl);
	if (!result.second)
		throw std::invalid_argument("listen directive duplicated");
}

void ServerContext::AddServerName(const std::string& name)
{
	if (!_serverName)
		_serverName = new std::set<std::string>();
	std::pair<std::set<std::string>::iterator, bool> result = _serverName->insert(name);
	if (!result.second)
		throw std::invalid_argument("server_name duplicated: " + name);
}

void ServerContext::SetCgiHandler(const std::string& ext, const std::string& interp)
{
	if (!_cgiHandlers)
		_cgiHandlers = new std::map<std::string, std::string>();

	std::pair<std::map<std::string, std::string>::iterator, bool> result =
		_cgiHandlers->insert(std::make_pair(ext, interp));

	if (!result.second)
		throw std::invalid_argument("cgi_handler duplicated for extension: " + ext);
}
