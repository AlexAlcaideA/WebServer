#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP
#include "includes.h"
class httpreq
{
	private:
		std::string agent;
		std::string host;
		std::string method;
		std::string version;
		std::string accept;
		std::string path;
		std::string lenght;

	protected:

	public:
// Destructor default
	~httpreq(void){};
// Constructor parametrizado
	httpreq(std::string text);
// Getters
	const std::string getAgent(void) const;
	const std::string getHost(void) const;
	const std::string getMethod(void) const;
	const std::string getVersion(void) const;
	const std::string getAccept(void) const;
	const std::string getPath(void) const;
	const std::string getLenght(void) const;
};
#endif
