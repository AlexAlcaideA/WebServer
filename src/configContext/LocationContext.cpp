#include "../../include/configContext/LocationContext.hpp"

LocationContext::LocationContext()
	: ConfigContext(), 
	_cgiHandlers(NULL), _limitExcept(NULL), _uploadStore(NULL), _returnVal(NULL)
{}

LocationContext::LocationContext(const LocationContext& other)
{
	*this = other;
}

LocationContext& LocationContext::operator=(const LocationContext& other)
{
	if (this != &other)
	{
		ConfigContext::operator=(other);
		delete _cgiHandlers;
		other._cgiHandlers == NULL ? _cgiHandlers = NULL : _cgiHandlers = new std::map<std::string, std::string>(*other._cgiHandlers);
		delete _limitExcept;
		other._limitExcept == NULL ? _limitExcept = NULL : _limitExcept = new std::vector<Method>(*other._limitExcept);
		delete _uploadStore;
		other._uploadStore == NULL ? _uploadStore = NULL : _uploadStore = new std::string(*other._uploadStore);
		delete _returnVal;
		other._returnVal == NULL ? _returnVal = NULL : _returnVal = new ReturnVal(*other._returnVal);
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

void LocationContext::SetCgiHandler(const std::string& ext, const std::string& interp)
{
	if (!_cgiHandlers)
		_cgiHandlers = new std::map<std::string, std::string>();

	std::pair<std::map<std::string, std::string>::iterator, bool> result =
		_cgiHandlers->insert(std::make_pair(ext, interp));

	if (!result.second)
		throw std::invalid_argument("cgi_handler duplicated for extension: " + ext);
}

void LocationContext::SetLimitExcept(const std::vector<Method>& methods)
{
	if (_limitExcept)
		throw std::invalid_argument("limit_except directive duplicated");
	_limitExcept = new std::vector<Method>(methods);
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
        _returnVal->url = nullptr;
}
