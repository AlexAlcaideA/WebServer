#include "client.hpp"

client::client(int fd)
	: _client_fd(fd), bytes(0), response(NULL), _keepAlive(false), _cgi(NULL)
{
	_rawData.clear();
}

client::~client(void)
{
	_cgi = NULL;
	if (_client_fd >= 0)
        close(_client_fd);
}

int client::getFd() const
{
	return (_client_fd);
}

const std::string&	client::getRawData() const
{
	return (_rawData);
}

ReceiveResult client::receive()
{
	char buffer[BUFF_SIZE];
	while (true)
	{
		ssize_t	n = recv(_client_fd, buffer, sizeof(buffer), 0);

		if (n == 0)
			return (RECV_CLOSED); // Client closed connection

		if (n < 0)
			return (RECV_INCOMPLETE);
		_rawData.append(buffer, n);

		// All headers?
		size_t headersEnd = _rawData.find("\r\n\r\n");
		if (headersEnd == std::string::npos)
			continue; // Continue reading headers

		// Parsear Content-Length
		size_t bodyStart = headersEnd + 4;
		size_t contentLength = 0;

		std::string headers = _rawData.substr(0, headersEnd);
		size_t clPos = headers.find("Content-Length:");
		if (clPos != std::string::npos)
        {
			clPos += 15; // strlen("Content-Length:")
			while (clPos < headers.size() && (headers[clPos] == ' ' || headers[clPos] == '\t'))
				++clPos;
			size_t clEnd = headers.find("\r\n", clPos);
			std::string clValue = headers.substr(clPos, clEnd - clPos);
			std::istringstream iss(clValue);
			iss >> contentLength;
		}
		if (_rawData.size() >= bodyStart + contentLength)
			return RECV_COMPLETE; // Reading complete
	}
	return (RECV_INCOMPLETE);
}

const ServerContext::ServerListen& client::GetListener() const
{
	return _listener;
}

bool client::getKeepAlive() const
{
	return _keepAlive;
}

void client::setKeepAlive(bool val)
{
	_keepAlive = val;
}

CgiHandler* client::getCgi() const
{
	return _cgi;
}

void client::setCgi(CgiHandler* cgi)
{
	_cgi = cgi;
}

bool client::hasCgi() const
{
	return _cgi != NULL;
}

const std::string& client::getClientIp() const
{
	return _clientIp;
}

void client::setClientIp(const std::string& ip)
{
	_clientIp = ip;
}

void client::addListener(const std::string& ip, unsigned int port)
{
	_listener.serverIp = ip;
	_listener.port = port;
}

void client::addListener(unsigned int port)
{
	addListener("0.0.0.0", port);
}

void client::clearRawData()
{
	_rawData.clear();
}

bool client::isWriting() const
{
	return _isWriting;
}

void client::prepareResponse(const std::string& response)
{
	_outBuffer = response;
	_outOffset = 0;
	_isWriting = true;
}

bool client::flushResponse()
{
	while (_outOffset < _outBuffer.size())
	{
		ssize_t sent = send(_client_fd,	_outBuffer.data() + _outOffset,	_outBuffer.size() - _outOffset, MSG_NOSIGNAL);
		if (sent <= 0)
			return false;   // error
		_outOffset += sent;
	}
	_isWriting = false;
	_outBuffer.clear();
	_outOffset = 0;
	return true;
}

void client::resetForNextRequest()
{
	clearRawData();
	_outBuffer.clear();
	_outOffset = 0;
	_isWriting = false;
}
