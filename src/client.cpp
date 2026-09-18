#include "client.hpp"

client::client(int fd)
	: _client_fd(fd), bytes(0), response(NULL)
{
	_rawData.clear();
}

client::~client(void)
{
/*	if (client_fd >= 0)
		close(client_fd);
*/}
int client::getFd() const
{
	return (_client_fd);
}
const std::string&	client::getRawData() const
{
	return (_rawData);
}
bool client::receive()
{
	char buffer[BUFF_SIZE];
	while (true)
	{
		ssize_t	n = recv(_client_fd, buffer, sizeof(buffer), 0);

		if (n == 0)
		{
			// Client closed connection
			return (false);
		}

		if (n < 0)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				break; // EAGAIN / EWOULDBLOCK
			return (false); // Error
		}
		_rawData.append(buffer, n);

		// All headers?
		size_t headersEnd = _rawData.find("\r\n\r\n");
		if (headersEnd == std::string::npos)
			continue; // Continue reading headers

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
			return true; // Reading complete
	}
	return (false);
}

const ServerContext::ServerListen& client::GetListener() const
{
	return _listener;
}

void client::AddListener(const std::string& ip, unsigned int port)
{
	_listener.serverIp = ip;
	_listener.port = port;
}

void client::AddListener(unsigned int port)
{
	AddListener("0.0.0.0", port);
}
