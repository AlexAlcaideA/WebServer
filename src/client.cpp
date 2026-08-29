#include "client.hpp"

// Constructor parametrizado
/*client::client()
{
	int client_fd = accept(Server.get_server_fd(), NULL, NULL);
	/*if (client_fd == -1)
	{
		perror("accept");
		kill(0);
	}*
	buffer(BUFF_SIZE, '\0');
	bytes = read(client_fd, buffer.c_str(), buffer.size() - 1);
/*	if (bytes <= 0)
	{
		close(client_fd);
	}*
}*/
client::client(int fd)
	: client_fd(fd),buffer(""), bytes(0), response(NULL)
{
}

client::~client(void)
{
	if (client_fd >= 0)
		close(client_fd);
}
int client::getFd() const
{
	return (client_fd);
}
char*	&client::getBuffer()
{
	return (&buffer);
}
bool client::receive()
{
	
	ssize_t	n;

	n = recv(client_fd, buffer, sizeof(buffer), 0);

	if (n == 0)
	{
		// Client closed connection
		return (false);
	}

	if (n < 0)
	{
		// Error / EAGAIN / EWOULDBLOCK
		return (false);
	}


	return (true);
}
