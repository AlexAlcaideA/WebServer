#include "../../../Includes/configuration/configContext/LocationContext.hpp"

LocationContext::LocationContext()
	: ConfigContext(),
	_cgiHandlers(NULL), _limitExcept(NULL), _uploadStore(NULL), _returnVal(NULL), _path("/")
{}

LocationContext::LocationContext(const std::string& path)
	: ConfigContext(),
	_cgiHandlers(NULL), _limitExcept(NULL), _uploadStore(NULL), _returnVal(NULL), _path(path)
{}

LocationContext::LocationContext(const LocationContext& other)
	: ConfigContext(other),
	_cgiHandlers(other._cgiHandlers ? new std::map<std::string, std::string>(*other._cgiHandlers) : NULL),
	_limitExcept(other._limitExcept ? new std::vector<Http::Method>(*other._limitExcept) : NULL),
	_uploadStore(other._uploadStore ? new std::string(*other._uploadStore) : NULL),
	_returnVal(NULL),
	_path(other._path)
{
	if (other._returnVal)
	{
		_returnVal = new ReturnVal;
		_returnVal->code = other._returnVal->code;
		_returnVal->url = (other._returnVal->url) ? new std::string(*other._returnVal->url) : NULL;
	}
}

LocationContext& LocationContext::operator=(const LocationContext& other)
{
	if (this != &other)
	{
		ConfigContext::operator=(other);
		delete _cgiHandlers;
		other._cgiHandlers == NULL ? _cgiHandlers = NULL : _cgiHandlers = new std::map<std::string, std::string>(*other._cgiHandlers);
		delete _limitExcept;
		other._limitExcept == NULL ? _limitExcept = NULL : _limitExcept = new std::vector<Http::Method>(*other._limitExcept);
		delete _uploadStore;
		other._uploadStore == NULL ? _uploadStore = NULL : _uploadStore = new std::string(*other._uploadStore);
		delete _returnVal;
		other._returnVal == NULL ? _returnVal = NULL : _returnVal = new ReturnVal(*other._returnVal);
		_path = other._path;
	}
	return *this;
}

LocationContext::~LocationContext()
{
	delete _cgiHandlers;
	delete _limitExcept;
	delete _uploadStore;
	if (_returnVal)
	{
		delete _returnVal->url;
		delete _returnVal;
	}
}

bool LocationContext::operator<(const LocationContext& other) const
{
	return _path < other._path;
}

bool LocationContext::operator==(const LocationContext& other) const
{
	return _path == other._path;
}

void LocationContext::SetCgiHandler(const std::string& ext, const std::string& interp)
{
	if (!_cgiHandlers)
		_cgiHandlers = new std::map<std::string, std::string>();

	std::pair<std::map<std::string, std::string>::iterator, bool> result =
		_cgiHandlers->insert(std::make_pair(ext, interp));

	if (!result.second)
		throw std::invalid_argument("cgi_handler duplicated for extension: " + ext);
}

void LocationContext::SetLimitExcept(const std::vector<Http::Method>& methods)
{
	if (_limitExcept)
		throw std::invalid_argument("limit_except directive duplicated");
	for (size_t i = 0; i < methods.size(); ++i)
	{
		for (size_t j = i + 1; j < methods.size(); ++j)
		{
			if (methods[i] == methods[j])
				throw std::invalid_argument("limit_except contains duplicate methods");
		}
	}
	_limitExcept = new std::vector<Http::Method>(methods);
}

void LocationContext::SetUploadStore(const std::string& path)
{
    if (_uploadStore)
        throw std::invalid_argument("upload_store directive duplicated");
    _uploadStore = new std::string(path);
}

void LocationContext::SetReturn(unsigned int code, const std::string* url)
{
    if (_returnVal)
        throw std::invalid_argument("return directive duplicated");
    _returnVal = new ReturnVal;
    _returnVal->code = code;
    if (url)
        _returnVal->url = new std::string(*url);
    else
        _returnVal->url = NULL;
}

void LocationContext::SetPath(const std::string& path)
{
	_path = path;
}

const std::string& LocationContext::GetPath() const
{
	return _path;
}

const std::map<std::string, std::string>* LocationContext::GetCgiHandlers() const
{
	return _cgiHandlers;
}

const std::string* LocationContext::GetCgiHandler(const std::string& extension) const
{
	if (!_cgiHandlers)
		return NULL;
	std::map<std::string, std::string>::const_iterator it = _cgiHandlers->find(extension);
	return (it != _cgiHandlers->end()) ? &it->second : NULL;
}

const std::vector<Http::Method>* LocationContext::GetLimitExcepts() const
{
	return _limitExcept;
}

const Http::Method* LocationContext::GetLimitExcept(size_t index) const
{
	if (!_limitExcept || index >= _limitExcept->size())
		return NULL;
	return &(*_limitExcept)[index];
}

const std::string* LocationContext::GetUploadStore() const
{
	return _uploadStore;
}

const LocationContext::ReturnVal* LocationContext::GetReturnVal() const
{
	return _returnVal;
}

std::ostream& operator<<(std::ostream& os, const LocationContext& other)
{
	other.print(os);

	os << "--Path: " << other.GetPath() << "\n";

	os << "--CGI handlers:\n";
	if (other.GetCgiHandlers() && !other.GetCgiHandlers()->empty())
	{
		std::map<std::string, std::string>::const_iterator it;
		for (it = other.GetCgiHandlers()->begin(); it != other.GetCgiHandlers()->end(); ++it)
			os << "----" << it->first << " -> " << it->second << "\n";
	}
	else
		os << "---Empty\n";

	os << "--Limit except: ";
	if (other.GetLimitExcepts())
	{
		for (size_t i = 0; i < other.GetLimitExcepts()->size(); ++i)
		{
			switch (*(other.GetLimitExcept(i)))
			{
				case Http::GET:
					os << "GET ";
					break;
				case Http::POST:
					os << "POST ";
					break;
				case Http::DELETE:
					os << "DELETE ";
					break;
				default:
					os << "Incorrect method.";
					break;
			}
		}
	}
	else
		os << "GET POST (default)";
	os << "\n";

	os << "--Upload store: ";
	if (other.GetUploadStore())
		os << *other.GetUploadStore();
	else
		os << "Empty";
	os << "\n";

	os << "--Return: ";
	if (other.GetReturnVal())
	{
		os << other.GetReturnVal()->code;
		if (other.GetReturnVal()->url)
			os << " " << *(other.GetReturnVal()->url);
	}
	else
		os << "Empty";
	os << "\n";

	return os;
}
