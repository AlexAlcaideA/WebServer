#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP
class (httpreq)
{
	private:
		agent
		host
		method
		version
		accept
		path

	protected:

	public:

// Destructor default
	~(httpreq)(void);
// Constructor default
	(httpreq)(void);
// Constructor parametrizado
	(httpreq)();
// Constructor copia
	(httpreq)(const (httpreq)& otro);
// Sobrecarga operador asignacion
	(httpreq) &operator= (const (httpreq)& otro);
};
#endif
