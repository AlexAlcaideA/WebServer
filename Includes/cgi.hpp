#ifndef CGI_HPP
# define CGI_HPP
class cgi
{
	private:
		char** envp;
	protected:

	public:

// Destructor default
	~cgi(void);
// Constructor default
	cgi(void);
// Constructor parametrizado
	cgi();
// Constructor copia
	cgi(const cgi& otro);
// Sobrecarga operador asignacion
	cgi &operator= (const cgi& otro);
};

#endif
