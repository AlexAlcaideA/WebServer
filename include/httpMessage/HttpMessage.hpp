#pragma once

#include "includes.hpp"

class HttpMessage
{
	protected:
		std::string _httpVersion;

		std::map<std::string, std::string> _headers;
		size_t _contentLenght;
		std::string* _content;

	public:
		// Constructor por defecto
		HttpMessage();
		// Constructor parametrizado
		HttpMessage(const std::string& httpVersion);
		HttpMessage(const std::string& httpVersion,	const std::map<std::string, std::string>& map);
		HttpMessage(const std::string& httpVersion,	const std::map<std::string, std::string>& map,
			size_t contentLenght, const std::string& content);
		HttpMessage(const HttpMessage& other);
		// Operadores
		HttpMessage& operator=(const HttpMessage& other);
		// Destructor default
		virtual ~HttpMessage();
		// Getters
		virtual const std::string& getHttpVersion() const;
		virtual const std::map<std::string, std::string>& getHeaders() const;
		virtual const std::string* getHeader(const std::string& key) const;
		virtual const std::string* getHeader(const HttpHeaders::Headers& header) const;
		virtual size_t getContentLenght() const;
		virtual const std::string* getContent() const;
		virtual std::string getStringMessage() const = 0;
		// Setters
		virtual void setHeader(const std::string& headerKey, const std::string& headerVal);
		virtual void setContentLenght(size_t lenght);
		virtual void setContent(const std::string& content);
};
