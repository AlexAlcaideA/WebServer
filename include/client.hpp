#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "includes.hpp"
# include "configuration/configContext/ServerContext.hpp"

class httpresponse;

enum ReceiveResult
{
	RECV_CLOSED,
	RECV_INCOMPLETE,
	RECV_COMPLETE
};

class client
{
	private:
		int							_client_fd;
		
		std::string					_rawData;
		std::string					_outBuffer;
		size_t						_outOffset;
		bool						_isWriting;
		size_t						bytes;
		httpresponse				*response;
		ServerContext::ServerListen _listener;
		bool						_keepAlive;

	public:
		client(int fd);
		~client();

		int	getFd() const;
		ReceiveResult		receive();
		const std::string&	getRawData() const;
		size_t	getBytes() const;
		const ServerContext::ServerListen& GetListener() const;
		bool	getKeepAlive() const;
		void	setKeepAlive(bool val);
		void	addListener(const std::string& ip, unsigned int port);
		void	addListener(unsigned int port);
		void	prepareResponse(const std::string& response);
		bool	flushResponse();
		bool	isWriting() const;
		void	clearRawData();
		void	resetForNextRequest();
};

#endif
