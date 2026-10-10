#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "includes.hpp"
# include "configuration/configContext/ServerContext.hpp"
# include "cgi/CgiHandler.hpp"

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

		CgiHandler*					_cgi;
		std::string					_clientIp;

	public:
		client(int fd);
		~client();

		int					getFd() const;
		ReceiveResult		receive();
		const std::string&	getRawData() const;
		size_t				getBytes() const;
		const ServerContext::ServerListen& GetListener() const;
		bool				getKeepAlive() const;
		void				setKeepAlive(bool val);
		CgiHandler*			getCgi() const;
		void				setCgi(CgiHandler* cgi);
		bool				hasCgi() const;
		const std::string&	getClientIp() const;
		void				setClientIp(const std::string& ip);
		void				addListener(const std::string& ip, unsigned int port);
		void				addListener(unsigned int port);
		void				prepareResponse(const std::string& response);
		bool				flushResponse();
		bool				isWriting() const;
		void				clearRawData();
		void				resetForNextRequest();
};

#endif
