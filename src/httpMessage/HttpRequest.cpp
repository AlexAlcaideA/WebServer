#include "../../include/httpMessage/HttpRequest.hpp"
#include "../../include/utils/StringUtils.hpp"

HttpRequest::HttpRequest()
	: HttpMessage(), _method(Http::UNKNOWN)
{}

HttpRequest::HttpRequest(const std::string& text)
	: HttpMessage()
{
	size_t lineEnd = text.find("\r\n");
	std::string requestLine = (lineEnd != std::string::npos) ? text.substr(0, lineEnd) : text;
	
	std::istringstream iss(requestLine);
	std::string methodStr, pathStr, versionStr;
	if (iss >> methodStr >> pathStr >> versionStr)
	{
		_requestTarget = pathStr;
		_httpVersion = versionStr;

		_method = Http::stringToMethod(methodStr);
		if (_method == Http::UNKNOWN)
			throw std::invalid_argument("Method not found or accepted.");
	}
	else
		throw std::invalid_argument("Incorrect request format.");

	size_t pos = lineEnd + 2;
	std::string headersBlock;

	size_t headersEnd = text.find("\r\n\r\n", pos);
	if (headersEnd != std::string::npos) // TMP tal vez poner un error por no poder leer los headers
	{
		headersBlock = text.substr(pos, headersEnd - pos);

		std::istringstream headersStream(headersBlock);
		std::string headerLine;
		while (std::getline(headersStream, headerLine))
		{
			if (!headerLine.empty() && headerLine[headerLine.size() - 1] == '\r')
				headerLine.erase(headerLine.size() - 1);

			size_t colonPos = headerLine.find(':');
			if (colonPos != std::string::npos)
			{
				std::string name = headerLine.substr(0, colonPos);
				std::string value = headerLine.substr(colonPos + 1);

				size_t firstNonSpace = value.find_first_not_of(" \t");
				if (firstNonSpace != std::string::npos)
					value = value.substr(firstNonSpace);
				else
					value = "";

				if (name == "Content-Lenght")
				{
					if (!utils::stringToUnsignedLong(value, _contentLenght))
						std::cerr << "Incorrect number for Content-Lenght: " << value << std::endl;
				}
				else
					_headers[name] = value;
			}
		}
		if (_method != Http::POST) // + PATCH && PUT if we add them
			return;
		// Message body only POST currently
		size_t bodyStart = headersEnd + 4;
		if (bodyStart < text.size())
		{
			std::string body = text.substr(bodyStart);
			std::cout << "Body request:\n" << body << std::endl; // TMP
			// TMP Procesar body según Content-Type y Content-Length
		}
	}
}

HttpRequest::HttpRequest(Http::Method method, const std::string& requestTarget, const std::string& httpVersion)
	: HttpMessage(httpVersion), _method(method), _requestTarget(requestTarget)
{}

HttpRequest::HttpRequest(Http::Method method, const std::string& requestTarget, const std::string& httpVersion,
	const std::map<std::string, std::string>& map, size_t contentLenght, const std::string& content)
		: HttpMessage(httpVersion, map, contentLenght, content), _method(method), _requestTarget(requestTarget)
{}

HttpRequest::HttpRequest(const HttpRequest& other)
	: HttpMessage(other)
{
	*this = other;
}

HttpRequest& HttpRequest::operator=(const HttpRequest& other)
{
	if (this != &other)
	{
		HttpMessage::operator=(other);
		_method = other._method;
		_requestTarget = other._requestTarget;
	}
	return *this;
}

HttpRequest::~HttpRequest()
{}

Http::Method HttpRequest::getMethod() const
{
	return _method;
}

const std::string& HttpRequest::getRequestTarget() const
{
	return _requestTarget;
}

std::string HttpRequest::getStringMessage() const
{
	std::ostringstream oss;

	// First line
	try
	{
		oss << Http::methodToString(_method) << " " << _requestTarget << " " << _httpVersion << "\r\n";
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

std::ostream& operator<<(std::ostream& os, const HttpRequest& other)
{
	os << other.getStringMessage();
	return os;
}
