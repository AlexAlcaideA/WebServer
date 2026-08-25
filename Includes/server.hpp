#ifndef SERVER_HPP
# define SERVER_HPP
class server
{
	private:
		int server_fd;
		sockaddr_in address;

	protected:

	public:

	server(int puerto);
// Destructor default
	~server(void);
// Constructor default
	server(void);
// Constructor copia
	server(const server& otro);
// Sobrecarga operador asignacion
	server &operator= (const server& otro);
};

#endif
