#include "../../include/httpMessage/HttpMessage.hpp"

HttpMessage::HttpMessage()
	: _contentLenght(0), _content(NULL), _contentHeaders(NULL)
{}

HttpMessage::HttpMessage(const std::string& httpVersion)
	: _httpVersion(httpVersion), _contentLenght(0), _content(NULL), _contentHeaders(NULL)
{}

HttpMessage::HttpMessage(const std::string& httpVersion,	const std::map<std::string, std::string>& map)
	: _httpVersion(httpVersion), _headers(map), _contentLenght(0), _content(NULL), _contentHeaders(NULL)
{}

HttpMessage::HttpMessage(const std::string& httpVersion, const std::map<std::string, std::string>& map,
	size_t contentLenght, const std::string& content)
		: _httpVersion(httpVersion), _headers(map), _contentLenght(contentLenght), _content(new std::string(content)), _contentHeaders(NULL)
{}

HttpMessage::HttpMessage(const std::string& httpVersion, const std::map<std::string, std::string>& map,
	size_t contentLenght, const std::string& content, const std::map<std::string, std::string>& contentHeaders)
		: _httpVersion(httpVersion), _headers(map), _contentLenght(contentLenght), _content(new std::string(content)),
		_contentHeaders(new std::map<std::string, std::string>(contentHeaders))
{}

HttpMessage::HttpMessage(const HttpMessage& other)
	: _contentLenght(0), _content(NULL), _contentHeaders(NULL)
{
	*this = other;
}

HttpMessage& HttpMessage::operator=(const HttpMessage& other)
{
	if (this != &other)
	{
		_httpVersion = other._httpVersion;
		_headers = other._headers;
		_contentLenght = other._contentLenght;
		if (_content != NULL)
			delete _content;
		(other._content) ? _content = new std::string(*(other._content)) : _content = NULL;
		if (_contentHeaders != NULL)
			delete _contentHeaders;
		(other._contentHeaders) ? _contentHeaders = new std::map<std::string, std::string>(*(other._contentHeaders)) : _contentHeaders = NULL;
	}
	return *this;
}

HttpMessage::~HttpMessage()
{
	if (_content)
		delete _content;
	if (_contentHeaders)
		delete _contentHeaders;
}

const std::string& HttpMessage::getHttpVersion() const
{
	return _httpVersion;
}

const std::map<std::string, std::string>& HttpMessage::getHeaders() const
{
	return _headers;
}

const std::string* HttpMessage::getHeader(const std::string& key) const
{
	std::map<std::string, std::string>::const_iterator it = _headers.find(key);
	if (it == _headers.end())
		return NULL;
	return &(it->second);
}

const std::string* HttpMessage::getHeader(const HttpHeaders::Headers& header) const
{
	std::string key;
	try
	{
		key = HttpHeaders::HeadersToString(header);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return NULL;
	}
	
	std::map<std::string, std::string>::const_iterator it = _headers.find(key);
	if (it == _headers.end())
		return NULL;
	return &(it->second);
}

size_t HttpMessage::getContentLenght() const
{
	return _contentLenght;
}

const std::string* HttpMessage::getContent() const
{
	return _content;
}

const std::map<std::string, std::string>* HttpMessage::getContentHeaders() const
{
	return _contentHeaders;
}

void HttpMessage::setHeader(const std::string& headerKey, const std::string& headerVal)
{
	_headers[headerKey] = headerVal;
}

void HttpMessage::setContentLenght(size_t lenght)
{
	_contentLenght = lenght;
}

void HttpMessage::setContent(const std::string& content)
{
	if (_content)
		delete _content;
	_content = new std::string(content);
}

void HttpMessage::setContentHeader(const std::string& headerKey, const std::string& headerVal)
{
	if (!_contentHeaders)
		_contentHeaders = new std::map<std::string, std::string>();
	(*_contentHeaders)[headerKey] = headerVal;
}
