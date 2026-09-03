#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP
# include "HttpMessage.hpp"

class HttpResponse : public HttpMessage
{
	private:
		size_t _statusCode;
		std::string _reasonPhrase;

	public:
// Constructor default
	HttpResponse();
// Constructor parametrizado
	HttpResponse(const std::string& httpVersion, const std::map<std::string, std::string>& map,
		size_t statusCode, const std::string& reasonPhrase);
	HttpResponse(const std::string& httpVersion, const std::map<std::string, std::string>& map,
		size_t contentLenght, const std::string& content, size_t statusCode, const std::string& reasonPhrase);
// Constructor copia
	HttpResponse(const HttpResponse& other);
// Sobrecarga operador asignacion
	HttpResponse& operator=(const HttpResponse& other);
// Destructor default
	~HttpResponse();

// Getters
	size_t getStatusCode() const;
	const std::string& getReasonPhrase() const;

	std::string getStringMessage() const;
// Setters
	void setStatusCode(HttpStatus::HttpStatusCode status);
	void setStatusCode(size_t code);
};

std::ostream& operator<<(std::ostream& os, const HttpResponse& other);

#endif
