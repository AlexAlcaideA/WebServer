#include "../../include/configContext/ServerContext.hpp"

ServerContext::ServerContext()
	: ConfigContext(),
	_listen(NULL), _serverName(NULL), _location(NULL), _cgiHandlers(NULL)
{}

ServerContext::ServerContext(const ServerContext& other)
	: ConfigContext(other),
	_listen(other._listen ? new std::set<ServerListen>(*other._listen) : NULL),
	_serverName(other._serverName ? new std::set<std::string>(*other._serverName) : NULL),
	_location(other._location ? new std::map<std::string, LocationContext>(*other._location) : NULL),
	_cgiHandlers(other._cgiHandlers ? new std::map<std::string, std::string>(*other._cgiHandlers) : NULL)
{}

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
		other._location == NULL ? _location = NULL : _location = new std::map<std::string, LocationContext>(*other._location);
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

std::ostream& ServerContext::operator<<(std::ostream& os) const
{
	ConfigContext::operator<<(os);

	os << "Listen: ";
	if (_listen && !_listen->empty())
	{
		std::set<ServerListen>::const_iterator it;
		for (it = _listen->begin(); it != _listen->end(); ++it)
			os << it->serverIp << ":" << it->port << " ";
	}
	else
		os << "INADDR_ANY:http (default)";
	os << "\n";

	os << "Server names: ";
	if (_serverName && !_serverName->empty())
	{
		std::set<std::string>::const_iterator it;
		for (it = _serverName->begin(); it != _serverName->end(); ++it)
			os << *it << " ";
	}
	else
		os << "(empty)";
	os << "\n";

	os << "Locations:\n";
	if (_location && !_location->empty())
	{
		std::map<std::string, LocationContext>::const_iterator it;
		for (it = _location->begin(); it != _location->end(); ++it)
		{
			os << "Location " << it->first << ":\n";
			it->second << os;
			os << "\n";
		}
	}
	else
		os << "Empty\n";

	os << "CGI handlers:\n";
	if (_cgiHandlers && !_cgiHandlers->empty())
	{
		std::map<std::string, std::string>::const_iterator it;
		for (it = _cgiHandlers->begin(); it != _cgiHandlers->end(); ++it)
			os << it->first << " -> " << it->second << "\n";
	}
	else
		os << "Empty\n";

	return os;
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

void ServerContext::AddLocation(const LocationContext& location)
{
	if (!_location)
		_location = new std::map<std::string, LocationContext>();

	std::pair<std::map<std::string, LocationContext>::iterator, bool> result
		= _location->insert(std::make_pair(location.GetPath(), location));
	if (!result.second)
		throw std::invalid_argument("duplicate location: " + location.GetPath());
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

std::map<std::string, LocationContext>* ServerContext::GetLocations() const
{
	return _location;
}

LocationContext* ServerContext::GetLocation(size_t index)
{
	if (!_location || index >= _location->size())
		return NULL;
	std::map<std::string, LocationContext>::iterator it = _location->begin();
	for (size_t i = 0; i < index; ++i)
		++it;
	return &(it->second);
}

LocationContext* ServerContext::GetLastLocation()
{
	if (!_location || _location->empty())
		return NULL;
	std::map<std::string, LocationContext>::iterator it = _location->end();
	--it;
	return &(it->second);
}
