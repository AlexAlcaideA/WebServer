#ifndef SERVER_HPP
# define SERVER_HPP

# include "includes.hpp"

class client;

class server
{
	private:
		int		server_fd;
		int		port;
		struct sockaddr_in	address;
		std::vector<struct pollfd>	poll_fds;
		std::vector<client>	clients;

		void	setupSocket();
		void	acceptClient();
		void	handleClient(size_t i);
		void	removeClient(size_t i);
		int     get_server_fd(void) const;
	public:
		server(int port);
		~server();
		server& operator=(const server&);
		server(const server& otro);
		void	run();
};
#endif
