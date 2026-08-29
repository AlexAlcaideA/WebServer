#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP
#include "includes.hpp"
class HttpRequest
{
	private:
		Http::Method _method;
		std::string _requestTarget;
		std::string _httpVersion;

		std::map<std::string, std::string> headers;
		size_t _contentLenght;
		std::string* _content;

	protected:

	public:
// Constructor parametrizado
	HttpRequest(const std::string& text);
// Destructor default
	~HttpRequest();
// Getters
	const Http::Method getMethod() const;
	const std::string getRequestTarget() const;
	const std::string getHttpVersion() const;

	const std::string* getHost() const;
	const std::string* getUserAgent() const;
	const std::string* getAccept() const;
	const std::string* getContentType() const;
	const size_t getContentLenght() const;
	const std::string getContent() const;

	const std::string getStringRequest() const;
};
#endif
