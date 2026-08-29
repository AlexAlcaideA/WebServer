#include "../../../Includes/configuration/configContext/ConfigContext.hpp"
#include <iterator>
#include "../../../Includes/utils/StringUtils.hpp"

void ConfigContext::print(std::ostream& os) const
{
	os << "Root: ";
	os << (!_root ? "Empty" : *_root) + "\n";
	os << "Index list:\n";
	if (_index)
	{
		for (size_t i = 0; i < _index->size(); i++)
			os << "-" << (*_index)[i] << "\n";
	}
	else
		os << "-Empty\n";
	os << "AutoIndex: " ;
	if (_autoIndex)
		os << (*_autoIndex ? "true" : "false");
	else
		os << "Empty";
	os << "\n";
	os << "Client max body: " << (!_clientMaxBodySize ? "Empty" : utils::unsignedLongLongToString(*_clientMaxBodySize)) << " bytes\n";
	os << "Error page:\n";
	if (_errorPage)
	{
		for (std::map<unsigned int, std::string>::const_iterator it = _errorPage->begin();
			it != _errorPage->end(); ++it)
			os << "-" << it->first << " -> " << it->second << "\n";
	}
	else
		os << "-Empty error page\n";
}

ConfigContext::ConfigContext()
	: _root(NULL), _index(NULL), _autoIndex(NULL), _clientMaxBodySize(NULL), _errorPage(NULL)
{}

ConfigContext::ConfigContext(const ConfigContext& other)
	: _root(other._root ? new std::string(*other._root) : NULL),
	_index(other._index ? new std::vector<std::string>(*other._index) : NULL),
	_autoIndex(other._autoIndex ? new bool(*other._autoIndex) : NULL),
	_clientMaxBodySize(other._clientMaxBodySize ? new unsigned long long(*other._clientMaxBodySize) : NULL),
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
		other._clientMaxBodySize == NULL ? _clientMaxBodySize = NULL : _clientMaxBodySize = new unsigned long long(*other._clientMaxBodySize);
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

void ConfigContext::SetClientMaxBodySize(const unsigned long long& size)
{
	if (!_clientMaxBodySize)
		_clientMaxBodySize = new unsigned long long(size);
	else
		throw std::invalid_argument("clientMaxBodySize directive duplicated: " + utils::unsignedLongLongToString(size));
}

void ConfigContext::AddErrorPage(unsigned int code, const std::string& page)
{
	if (!_errorPage)
		_errorPage = new std::map<unsigned int, std::string>();
	(*_errorPage)[code] = page;
}

const std::string* ConfigContext::GetRoot() const
{
	return _root;
}

const std::vector<std::string>* ConfigContext::GetIndexes() const
{
	return _index;
}

const std::string* ConfigContext::GetIndex(size_t index) const
{
	if (!_index || index >= _index->size())
		return NULL;
	return &((*_index)[index]);
}

const bool* ConfigContext::GetAutoIndex() const
{
	return _autoIndex;
}

const unsigned long long* ConfigContext::GetClientMaxBodySize() const
{
	return _clientMaxBodySize;
}

const std::map<unsigned int, std::string>* ConfigContext::GetErrorPages() const
{
	return _errorPage;
}

const std::string* ConfigContext::GetErrorPage(unsigned int error) const
{
	return &((*_errorPage)[error]);
}

const std::pair<const unsigned int, std::string>* ConfigContext::GetErrorPageIndex(unsigned int index) const
{
	if (!_errorPage || index >= _errorPage->size())
		return NULL;
	std::map<unsigned int, std::string>::const_iterator it = _errorPage->begin();
	std::advance(it, index);
	return &(*it);
}

const std::string* ConfigContext::GetCgiHandler(const std::string& extension) const
{
	return &((*_cgiHandler)[extension]);
}

const std::pair<const std::string, std::string>* ConfigContext::GetCgiHandlerIndex(unsigned int index) const
{
	if (!_cgiHandler || index >= _cgiHandler->size())
		return NULL;
	std::map<std::string, std::string>::const_iterator it = _cgiHandler->begin();
	std::advance(it, index);
	return &(*it);
}

std::ostream& operator<<(std::ostream& os, const ConfigContext& other)
{
	other.print(os);
	return os;
}
