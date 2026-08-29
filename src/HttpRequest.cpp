#include "../Includes/HttpRequest.hpp"
#include "../Includes/utils/StringUtils.hpp"

HttpRequest::HttpRequest(const std::string& text)
	: _contentLenght(0), _content(NULL)
{
	size_t lineEnd = text.find("\r\n");
	std::string requestLine = (lineEnd != std::string::npos) ? text.substr(0, lineEnd) : text;
	
	std::istringstream iss(requestLine);
	std::string methodStr, pathStr, versionStr;
	if (iss >> methodStr >> pathStr >> versionStr)
	{
		_requestTarget = pathStr;
		_httpVersion = versionStr;

		if (methodStr == "GET")
			_method = Http::GET;
		else if (methodStr == "POST")
			_method = Http::POST;
		else if (methodStr == "DELETE")
			_method = Http::DELETE;
		else
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
			if (!headerLine.empty() && headerLine.back() == '\r')
				headerLine.pop_back();

			size_t colonPos = headerLine.find(':');
			if (colonPos != std::string::npos)
			{
				std::string name = headerLine.substr(0, colonPos);
				std::string value = headerLine.substr(colonPos + 1);
				if (name == "Content-Lenght")
				{
					if (!utils::stringToUnsignedLong(value, _contentLenght))
						std::cerr << "Incorrect number for Content-Lenght: " << value << std::endl;
				}
				else
					headers[name] = value;
			}
		}
		if (_method != Http::POST) // PATCH && PUT
			return;
		// Message body only POST currently
		size_t bodyStart = headersEnd + 4;
		if (bodyStart < text.size())
		{
			std::string body = text.substr(bodyStart);
			std::cout << "Body request:\n" << body << std::endl;
			// TMP Procesar body según Content-Type y Content-Length
		}
	}
}

HttpRequest::~HttpRequest()
{
	if (_content)
		delete _content;
}

const Http::Method HttpRequest::getMethod() const
{
	return _method;
}

const std::string HttpRequest::getRequestTarget() const
{
	return _requestTarget;
}

const std::string HttpRequest::getHttpVersion() const
{
	return _httpVersion;
}

const std::string* HttpRequest::getHost() const
{
	std::map<std::string, std::string>::const_iterator it = headers.find("Host");
	if (it == headers.end())
		return NULL;
	return &(it->second);
}

const std::string* HttpRequest::getUserAgent() const
{
	std::map<std::string, std::string>::const_iterator it = headers.find("User-Agent");
	if (it == headers.end())
		return NULL;
	return &(it->second);
}

const std::string* HttpRequest::getAccept() const
{
	std::map<std::string, std::string>::const_iterator it = headers.find("Accept");
	if (it == headers.end())
		return NULL;
	return &(it->second);
}

const std::string* HttpRequest::getContentType() const
{
	std::map<std::string, std::string>::const_iterator it = headers.find("Content-Type");
	if (it == headers.end())
		return NULL;
	return &(it->second);
}

const size_t HttpRequest::getContentLenght() const
{
	return _contentLenght;
}

const std::string HttpRequest::getContent() const
{
	return *_content;
}

const std::string HttpRequest::getStringRequest() const
{
	std::ostringstream oss;

	// First line
	oss << Http::methodToString(_method) << " " << _requestTarget << " " << _httpVersion << "\r\n";
	// Headers
	for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it)
    	oss << it->first << ": " << it->second << "\r\n";
	// End of headers
	oss << "\r\n";
	// Body
	if (_content && !_content->empty())
		oss << *_content;
	return oss.str();
}
