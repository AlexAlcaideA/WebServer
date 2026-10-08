#ifndef SERVER_HPP
# define SERVER_HPP

# include "includes.hpp"
# include "configuration/Configuration.hpp"
# include "httpMessage/HttpRequest.hpp"
# include "httpMessage/HttpResponse.hpp"
# include "client.hpp"
# include "cgi/CgiManager.hpp"

class client;

class server
{
	private:
		Configuration*							_conf;
		CgiManager								_cgiManager;
		std::vector<struct sockaddr_in>			_address;
		std::vector<struct pollfd>				_pollFds;
		std::vector<struct pollfd>				_newPollFds;
		std::vector<int>						_listenSockets;
		std::vector<client*>					_clients;

		void									setupSocket();
		void									acceptClient();
		HttpResponse							methodGet(const HttpRequest& req, const LocationContext& loc);
		HttpResponse 							methodPost(const HttpRequest& req, const LocationContext& loc);
		HttpResponse 							methodDelete(const HttpRequest& req, const LocationContext& loc);
		HttpResponse							methodNotImplemented(const LocationContext& loc);
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
		void									acceptNewClient(int listenFd);
		void									readFromClient(size_t index);
		void									writeToClient(size_t index);
		void									handleCgi(client* cli, const HttpRequest& req, const ServerContext& serv, const LocationContext& loc,
													const ServerContext::ServerListen& listen);
		void									handleCgiFinished(CgiHandler* cgi);
	public:
		server();
		server(const Configuration& conf);
		~server();
		server& operator=(const server&);
		server(const server& otro);
		void	run();
};
#endif
