#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP
class httpresponse
{
	private:

		std::string version
		std::string status
		std::string type
		int lenght
		conection
		std::string content
	protected:

	public:

// Destructor default
	~httpresponse(void);
// Constructor default
	httpresponse(void);
// Constructor parametrizado
	httpresponse();
// Constructor copia
	httpresponse(const httpresponse& otro);
// Sobrecarga operador asignacion
	httpresponse &operator= (const httpresponse& otro);
};
#endif
