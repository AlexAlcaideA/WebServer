#ifndef SERVER_HPP
# define SERVER_HPP

# include "includes.hpp"
# include "configuration/Configuration.hpp"
# include "httpMessage/HttpRequest.hpp"
# include "httpMessage/HttpResponse.hpp"
# include "client.hpp"

class client;

class server
{
	private:
		int		server_fd;
		int		port;
		Configuration* _conf;
		struct sockaddr_in	address;
		std::vector<struct sockaddr_in>	_address;
		std::vector<struct pollfd> poll_fds;
		std::vector<int> _listenSockets;
		std::vector<client*>	clients;

		void									setupSocket();
		void									acceptClient();
		HttpResponse							methodGet(const HttpRequest& req, const client& currentClient);
		const ServerContext*					getServerByName(const std::string& name,
													const std::string& ip, unsigned int port) const;
		void									handleClient(int fd);
		void									removeClient(size_t i);
		int										get_server_fd(void) const;
		bool									isListenSocket(int fd) const;
		client*									findClientByFd(int fd);
		void									removeClientByFd(int fd);
		std::pair<std::string, unsigned short>	getLocalAddressInfo(int clientFd);
		bool									checkLocalMethods(Http::Method method, const LocationContext& local);
	public:
		//server(int port);
		server(const Configuration& conf);
		~server();
		server& operator=(const server&);
		server(const server& otro);
		void	run();
};
#endif
