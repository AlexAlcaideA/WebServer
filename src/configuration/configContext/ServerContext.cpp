#include "../../../include/configuration/configContext/ServerContext.hpp"
#include <iterator>

bool ServerContext::isValidIPv4(const std::string& ip)
{
	struct addrinfo hints, *res;
	std::memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;       // solo IPv4
	hints.ai_socktype = SOCK_STREAM; // no importa para validar
	hints.ai_flags = AI_NUMERICHOST; // no resolver nombres

	int status = getaddrinfo(ip.c_str(), NULL, &hints, &res);
	if (status != 0)
		return false;
	freeaddrinfo(res);
	return true;
}

bool ServerContext::isValidPort(unsigned int port)
{
	return port > 0 && port <= 65535;
}

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

void ServerContext::SetListen(const unsigned int& port)
{
	if (!isValidPort(port))
		throw std::invalid_argument("Invalid port number.");
	SetListen("0.0.0.0", port);
}

void ServerContext::SetListen(const std::string& ip, const unsigned int& port)
{
	if (!isValidPort(port))
		throw std::invalid_argument("Invalid port number.");
	if (!isValidIPv4(ip))
		throw std::invalid_argument("Invalid ip " + ip);

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

const std::set<ServerContext::ServerListen>* ServerContext::GetListens() const
{
	return _listen;
}

const ServerContext::ServerListen* ServerContext::GetListen(size_t index) const
{
	if (!_listen || index >= _listen->size())
		return NULL;
	std::set<ServerContext::ServerListen>::const_iterator  it = _listen->begin();
	for (size_t i = 0; i < index; ++i)
		++it;
	return &(*it);
}

const std::set<std::string>* ServerContext::GetServerNames() const
{
	return _serverName;
}

const std::string* ServerContext::GetServerName(size_t index) const
{
	if (!_serverName || index >= _serverName->size())
		return NULL;
	std::set<std::string>::const_iterator  it = _serverName->begin();
	for (size_t i = 0; i < index; ++i)
		++it;
	return &(*it);
}

const std::map<std::string, LocationContext>* ServerContext::GetLocations() const
{
	return _location;
}

size_t ServerContext::GetLocationsSize() const
{
	return _location->size();
}

const LocationContext* ServerContext::GetLocation(size_t index) const
{
	if (!_location || index >= _location->size())
		return NULL;
	std::map<std::string, LocationContext>::const_iterator it = _location->begin();
	for (size_t i = 0; i < index; ++i)
		++it;
	return &(it->second);
}

LocationContext& ServerContext::GetLocation(size_t index)
{
	std::map<std::string, LocationContext>::iterator it = _location->begin();
	std::advance(it, index);
	return (it->second);
}

LocationContext* ServerContext::GetLastLocation()
{
	if (!_location || _location->empty())
		return NULL;
	std::map<std::string, LocationContext>::iterator it = _location->end();
	--it;
	return &(it->second);
}

const LocationContext* ServerContext::GetLastLocation() const
{
	if (!_location || _location->empty())
		return NULL;
	std::map<std::string, LocationContext>::const_iterator it = _location->end();
	--it;
	return &(it->second);
}

const std::map<std::string, std::string>* ServerContext::GetCgiHandlers() const
{
	return _cgiHandlers;
}

const std::string* ServerContext::GetCgiHandler(const std::string& extension) const
{
	if (!_cgiHandlers)
		return NULL;
	std::map<std::string, std::string>::const_iterator it = _cgiHandlers->find(extension);
	return (it != _cgiHandlers->end()) ? &it->second : NULL;
}

std::ostream& operator<<(std::ostream& os, const ServerContext& other)
{
	other.print(os);

	os << "-Listen: ";
	if (other.GetListens() && !other.GetListens()->empty())
	{
		std::set<ServerContext::ServerListen>::const_iterator it;
		for (it = other.GetListens()->begin(); it != other.GetListens()->end(); ++it)
			os << it->serverIp << ":" << it->port << " ";
	}
	else
		os << "INADDR_ANY:http (default)";
	os << "\n";

	os << "-Server names: ";
	if (other.GetServerNames() && !other.GetServerNames()->empty())
	{
		std::set<std::string>::const_iterator it;
		for (it = other.GetServerNames()->begin(); it != other.GetServerNames()->end(); ++it)
			os << *it << " ";
	}
	else
		os << "(empty)";
	os << "\n";

	os << "-Locations:\n";
	if (other.GetLocations() && !other.GetLocations()->empty())
	{
		std::map<std::string, LocationContext>::const_iterator it;
		for (it = other.GetLocations()->begin(); it != other.GetLocations()->end(); ++it)
		{
			os << "--Location " << it->first << ":\n";
			os << it->second;
			os << "\n";
		}
	}
	else
		os << "Empty\n";

	os << "-CGI handlers:\n";
	if (other.GetCgiHandlers() && !other.GetCgiHandlers()->empty())
	{
		std::map<std::string, std::string>::const_iterator it;
		for (it = other.GetCgiHandlers()->begin(); it != other.GetCgiHandlers()->end(); ++it)
			os << it->first << " -> " << it->second << "\n";
	}
	else
		os << "--Empty\n";

	return os;
}
