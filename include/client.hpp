#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "includes.hpp"

class httpresponse;

class client
{
	private:
		int		client_fd;
		char		buffer[BUFF_SIZE];
		size_t		bytes;
		httpresponse	*response;

	public:
		client(int fd);
		~client();

		int	getFd() const;
		bool		receive();
		char (*getBuffer())[BUFF_SIZE];
		size_t	getBytes() const;
};

#endif
