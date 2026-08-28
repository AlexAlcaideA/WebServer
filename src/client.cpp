#include "client.hpp"
// Destructor default
client::~client(void)
{
	close(client_fd);
}
// Constructor parametrizado
client::client()
{
	int client_fd = accept(Server.get_server_fd(), NULL, NULL);
	/*if (client_fd == -1)
	{
		perror("accept");
		kill(0);
	}*/
	buffer(BUFF_SIZE, '\0');
	bytes = read(client_fd, buffer.c_str(), buffer.size() - 1);
/*	if (bytes <= 0)
	{
		close(client_fd);
	}*/
}
