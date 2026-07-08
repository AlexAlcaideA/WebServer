#include "../../include/configContext/ConfigContext.hpp"

ConfigContext::ConfigContext()
	: _root(NULL), _index(NULL), _autoIndex(NULL), _clientMaxBodySize(NULL), _errorPage(NULL)
{}

ConfigContext::ConfigContext(const ConfigContext& other)
	: _root(other._root ? new std::string(*other._root) : NULL),
	_index(other._index ? new std::vector<std::string>(*other._index) : NULL),
	_autoIndex(other._autoIndex ? new bool(*other._autoIndex) : NULL),
	_clientMaxBodySize(other._clientMaxBodySize ? new std::string(*other._clientMaxBodySize) : NULL),
	_errorPage(other._errorPage ? new std::map<unsigned int, std::string>(*other._errorPage) : NULL)
{}

ConfigContext& ConfigContext::operator=(const ConfigContext& other)
{
	if (this != &other)
	{
		delete _root;
		other._root == NULL ? _root = NULL : _root = new std::string(*other._root);
		delete _index;
		other._index == NULL ? _index = NULL : _index = new std::vector<std::string>(*other._index);
		delete _autoIndex;
		other._autoIndex == NULL ? _autoIndex = NULL : _autoIndex = new bool(*other._autoIndex);
		delete _clientMaxBodySize;
		other._clientMaxBodySize == NULL ? _clientMaxBodySize = NULL : _clientMaxBodySize = new std::string(*other._clientMaxBodySize);
		delete _errorPage;
		other._errorPage == NULL ? _errorPage = NULL : _errorPage = new std::map<unsigned int, std::string>(*other._errorPage);
	}
	return *this;
}

ConfigContext::~ConfigContext()
{
	delete _root;
	delete _index;
	delete _autoIndex;
	delete _clientMaxBodySize;
	delete _errorPage;
}

std::ostream& ConfigContext::operator<<(std::ostream& os) const
{
	os << "Root: ";
	os << (!_root ? "Empty" : *_root) + "\n";
	os << "Index list:\n";
	if (_index)
	{
		for (size_t i = 0; i < _index->size(); i++)
			os << (*_index)[i] << "\n";
	}
	else
		os << "Empty\n";
	if (_autoIndex)
		os << (*_autoIndex ? "true" : "false");
	else
		os << "Empty";
	os << "Client max body: " << (!_clientMaxBodySize ? "Empty" : *_clientMaxBodySize) << "\n";
	os << "Error page:\n";
	if (_errorPage)
	{
		for (std::map<unsigned int, std::string>::const_iterator it = _errorPage->begin();
			it != _errorPage->end(); ++it)
			os << it->first << " -> " << it->second << "\n";
	}
	else
		os << "Empty error page\n";
	
	return os;
}

void ConfigContext::SetRoot(const std::string& path)
{
	if (!_root)
		_root = new std::string(path);
	else
		throw std::invalid_argument("root directive duplicated: " + path);
}

void ConfigContext::AddIndex(const std::string& file)
{
	if (!_index)
		_index = new std::vector<std::string>();
	_index->push_back(file);
}

void ConfigContext::SetAutoIndex(bool on)
{
	if (!_autoIndex)
		_autoIndex = new bool(on);
	else
		throw std::invalid_argument("autoIndex directive duplicated");
}

void ConfigContext::SetClientMaxBodySize(const std::string& size)
{
	if (!_clientMaxBodySize)
		_clientMaxBodySize = new std::string(size);
	else
		throw std::invalid_argument("clientMaxBodySize directive duplicated: " + size);
}

void ConfigContext::AddErrorPage(unsigned int code, const std::string& page)
{
	if (!_errorPage)
		_errorPage = new std::map<unsigned int, std::string>();
	(*_errorPage)[code] = page;
}
