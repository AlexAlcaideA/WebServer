#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP
#include "HttpMessage.hpp"
class HttpRequest : public HttpMessage
{
	private:
		Http::Method _method;
		std::string _requestTarget;

	public:
// Constructor por defecto
	HttpRequest();
// Constructor parametrizado
	HttpRequest(const std::string& text);
	HttpRequest(Http::Method method, const std::string& requestTarget, const std::string& httpVersion);
	HttpRequest(Http::Method method, const std::string& requestTarget, const std::string& httpVersion,
		const std::map<std::string, std::string>& map, size_t contentLenght, const std::string& content);
	HttpRequest(const HttpRequest& other);
// Operadores
	HttpRequest& operator=(const HttpRequest& other);
// Destructor default
	~HttpRequest();
// Getters
	Http::Method getMethod() const;
	const std::string& getRequestTarget() const;

	std::string getStringMessage() const;
};

std::ostream& operator<<(std::ostream& os, const HttpRequest& other);

#endif
