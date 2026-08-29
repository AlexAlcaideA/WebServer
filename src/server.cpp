#include "server.hpp"
#include "includes.hpp"

// Destructor default
server::~server(void)
{
	close(this->server_fd);
}
// Constructor copia
server::server(const server& otro):server_fd(otro.server_fd), address(otro.address)
{
}
// Sobrecarga operador asignacion
server &server::operator= (const server& otro)
{
	if (this == &otro)
	{
		return (*this);
	}
//Copia miembros
	this->server_fd = otro.server_fd;
	this->address = otro.address;
	return (*this);
}

int	server::get_server_fd(void) const
{
	return (server_fd);
}
//Set socket para constructor
void	server::setupSocket()
{
	int opt = 1;

	struct protoent *protocolo = getprotobyname("tcp");
	this->server_fd = socket(AF_INET, SOCK_STREAM, protocolo->p_proto);
	if (server_fd == -1)
		throw std::runtime_error("socket failed");

	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR,
		&opt, sizeof(opt));

	if (bind(server_fd,
			(struct sockaddr *)&address,
			sizeof(address)) == -1)
	{
		close(server_fd);
		throw std::runtime_error("bind failed");
	}

	if (listen(server_fd, SOMAXCONN) == -1)
	{
		close(server_fd);
		throw std::runtime_error("listen failed");
	}
}
// Constructor parametrizado
	server::server(int port = 8080)
	: server_fd(-1), port(port)
{
	std::memset(&address, 0, sizeof(address));

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(port);

	setupSocket();
	std::cout << "Server listening on port " << port << std::endl;
}
void server::acceptClient()
{
	int fd;

	fd = accept(server_fd, NULL, NULL);
	if (fd == -1)
	{
		std::cerr << "accept: " << strerror(errno) << std::endl;
		return;
	}
	fcntl(fd, F_SETFL, O_NONBLOCK);

	std::cout << "New client: " << fd << std::endl;

	client new_client(fd);
	clients.push_back(new_client);

	struct pollfd pfd;
	pfd.fd = fd;
	pfd.events = POLLIN;
	pfd.revents = 0;

	poll_fds.push_back(pfd);
}
void server::handleClient(size_t i)
{
	if (!clients[i].receive())
	{
		removeClient(i);
		return;
	}

	std::cout << "Received:\n";
	std::cout << clients[i].getBuffer();

	DefaultResponse ans;
	std::string answer = ans.getString(SUCCESS);
	send(clients[i].getFd(), answer.c_str(), answer.length(), 0);

	removeClient(i);
}

void server::run()
{
	while (1)
	{
		if (poll(&poll_fds[0], poll_fds.size(), -1) == -1)
			throw std::runtime_error("poll failed");

		for (size_t i = 0; i < poll_fds.size(); i++)
		{
			if (poll_fds[i].revents == 0)
				continue;

			if (poll_fds[i].fd == server_fd)
			{
				acceptClient();
			}
			else
			{
				handleClient(i);
			}
		}
	}
}
void	server::removeClient(unsigned long i)
{
	close(poll_fds[i].fd);
	poll_fds.erase(poll_fds.begin() + i);
	clients.erase(clients.begin() + (i - 1));
}
