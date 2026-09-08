#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "includes.hpp"
# include "configuration/configContext/ServerContext.hpp"

class httpresponse;

class client
{
	private:
		int							_client_fd;
		char						buffer[BUFF_SIZE];
		size_t						bytes;
		httpresponse				*response;
		ServerContext::ServerListen _listener;

	public:
		client(int fd);
		~client();

		int	getFd() const;
		bool		receive();
		char (*getBuffer())[BUFF_SIZE];
		size_t	getBytes() const;
		const ServerContext::ServerListen& GetListener() const;
		void	AddListener(const std::string& ip, unsigned int port);
		void	AddListener(unsigned int port);
};

#endif
