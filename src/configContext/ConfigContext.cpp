#include "../../include/configContext/ConfigContext.hpp"

ConfigContext::ConfigContext()
	: _root(NULL), _index(NULL), _autoIndex(NULL), _clientMaxBodySize(NULL), _errorPage(NULL)
{}

ConfigContext::ConfigContext(const ConfigContext& other)
{
	*this = other;
}

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
