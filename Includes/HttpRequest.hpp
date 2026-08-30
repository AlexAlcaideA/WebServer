#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP
#include "includes.hpp"
class HttpRequest
{
	private:
		Http::Method _method;
		std::string _requestTarget;
		std::string _httpVersion;

		std::map<std::string, std::string> _headers;
		size_t _contentLenght;
		std::string* _content;

	protected:

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
	const std::string& getHttpVersion() const;

	const std::map<std::string, std::string>& getHeaders() const;
	const std::string* getHeader(const std::string& key) const;
	const std::string* getHeader(const HttpHeaders::Headers& header) const;
	size_t getContentLenght() const;
	const std::string* getContent() const;

	std::string getStringRequest() const;
// Setters
	void setHeader(const std::string& headerKey, const std::string& headerVal);
	void setContentLenght(size_t lenght);
	void setContent(const std::string& content);
};

std::ostream& operator<<(std::ostream& os, const HttpRequest& other);

#endif
