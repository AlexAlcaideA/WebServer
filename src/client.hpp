#ifndef CLIENT_HPP
# define CLIENT_HPP
class client
{
	private:
		int client_fd;
		std::string buffer;
		int bytes;
	protected:

	public:

// Destructor default
	~client(void){close(client_fd);};
// Constructor parametrizado
	client();
};
#endif
