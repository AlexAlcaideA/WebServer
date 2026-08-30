#include "client.hpp"

client::client(int fd)
	: client_fd(fd), bytes(0), response(NULL)
{
	std::memset(buffer, 0, sizeof(buffer));
}

client::~client(void)
{
/*	if (client_fd >= 0)
		close(client_fd);
*/}
int client::getFd() const
{
	return (client_fd);
}
char	(*client::getBuffer())[BUFF_SIZE]
{
	return (&buffer);
}
bool client::receive()
{
	
	ssize_t	n;

	n = recv(client_fd, buffer, sizeof(buffer) -1, 0);

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
	buffer[n] = '\0';

	return (true);
}
