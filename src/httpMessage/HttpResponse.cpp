#include "../../include/httpMessage/HttpResponse.hpp"

HttpResponse::HttpResponse()
	: HttpMessage(), _statusCode(0), _reasonPhrase(HttpStatus::reasonPhrase(0))
{}

HttpResponse::HttpResponse(const std::string& httpVersion, const std::map<std::string, std::string>& map,
		size_t statusCode, const std::string& reasonPhrase)
			: HttpMessage(httpVersion, map), _statusCode(statusCode), _reasonPhrase(reasonPhrase)
{}

HttpResponse::HttpResponse(const std::string& httpVersion, const std::map<std::string, std::string>& map,
		size_t contentLenght, const std::string& content, size_t statusCode, const std::string& reasonPhrase)
			: HttpMessage(httpVersion, map, contentLenght, content), _statusCode(statusCode), _reasonPhrase(reasonPhrase)
{}

HttpResponse::HttpResponse(const HttpResponse& other)
	: HttpMessage(other)
{
	*this = other;
}

HttpResponse& HttpResponse::operator=(const HttpResponse& other)
{
	if (this != &other)
	{
		HttpMessage::operator=(other);
		_statusCode = other._statusCode;
		_reasonPhrase = other._reasonPhrase;
	}
	return *this;
}

HttpResponse::~HttpResponse()
{}

size_t HttpResponse::getStatusCode() const
{
	return _statusCode;
}

const std::string& HttpResponse::getReasonPhrase() const
{
	return _reasonPhrase;
}

std::string HttpResponse::getStringMessage() const
{
	std::ostringstream oss;

	// First line
	try
	{
		oss << _httpVersion << " " << utils::unsignedLongToString(_statusCode) << " " << _reasonPhrase << "\r\n";
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	
	// Headers
	for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
    	oss << it->first << ": " << it->second << "\r\n";
	// End of headers
	oss << "\r\n";
	// Body
	if (_content && !_content->empty())
		oss << *_content;
	return oss.str();
}

void HttpResponse::setStatusCode(HttpStatus::HttpStatusCode status)
{
	_statusCode = status;
	_reasonPhrase = HttpStatus::reasonPhrase(status);
}

void HttpResponse::setStatusCode(size_t code)
{
	_statusCode = code;
	_reasonPhrase = HttpStatus::reasonPhrase(code);
}

std::ostream& operator<<(std::ostream& os, const HttpResponse& other)
{
	os << other.getStringMessage();
	return os;
}
