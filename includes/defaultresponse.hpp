#ifndef DEFAULTRESPONSE_HPP
# define DEFAULTRESPONSE_HPP
#include <string>

class DefaultResponse {
public:
	enum class Type {
		SUCCESS = 200,
		CREATED = 201,
		NO_CONTENT = 204,
		MOVED_PERMANENTLY = 301,
		FOUND = 302,
		NOT_MODIFIED = 304,
		BAD_REQUEST = 400,
		UNAUTHORIZED = 401,
		FORBIDDEN = 403,
		NOT_FOUND = 404,
		METHOD_NOT_ALLOWED = 405,
		TEAPOT = 418,
		INTERNAL_ERROR = 500,
		SERVICE_DOWN = 503
	};

	explicit DefaultResponse(Type type) : type_(type) {}

	std::string getString() const {
		switch (type_) {
			case Type::200:
				return ("HTTP/1.1 200 OK\r\n"
				"Content-Type: text/plain\r\n"
				"Content-Length: 5\r\n"
				"\r\n"
				"Hello");
			case Type::201:
				return ("HTTP/1.1 201 Created\r\n"
				"Location: /users/123\r\n"
				"Content-Length: 0\r\n");
			case Type::204:
				return ("HTTP/1.1 204 No Content");
			case Type::301:
				return ("HTTP/1.1 301 Moved Permanently\r\n"
				"Location: https://www.youtube.com/watch?v=dQw4w9WgXcQ\r\n"
				"Content-Length: 0\r\n");
			case Type::302:
				return ("HTTP/1.1 302 Found\r\n"
				"Location: /login\r\n"
				"Content-Length: 0\r\n");
			case Type::304:
				return ("HTTP/1.1 304 Not Modified\r\n"
				"ETag: \"Non modified\"\r\n");
			case Type::400:
				return ("HTTP/1.1 400 Bad Request\r\n"
				"Content-Type: text/plain\r\n"
				"Content-Length: 11\r\n"
				"\r\n"
				"Bad Request\r\n");
			case Type::401:
				return ("HTTP/1.1 401 Unauthorized\r\n"
				"WWW-Authenticate: Basic realm=\"Example\"\r\n"
				"Content-Length: 0\r\n");
			case Type::403:
				return ("HTTP/1.1 403 Forbidden\r\n"
				"Content-Type: text/plain\r\n"
				"Content-Length: 9\r\n"
				"\r\n"
				"Forbidden\r\n");
			case Type::404:
				return ("HTTP/1.1 404 Not Found\r\n"
				"Content-Length: 13\r\n"
				"Content-Type: text/plain\r\n"
				"Connection: close\r\n"
				"\r\n"
				"404 Not Found");
			case Type::405:
				return ("HTTP/1.1 405 Method Not Allowed\r\n"
				"Content-Length: 18\r\n"
				"Content-Type: text/plain\r\n"
				"Connection: close\r\n"
				"\r\n"
				"Method Not Allowed");
			case Type::418:
				return ("HTTP/1.1 418 I'm a teapot\r\n"
				"Content-Length: 14\r\n"
				"\r\n"
				"I'm a teapot\r\n");
			case Type::500:
				return ("HTTP/1.1 500 Internal Server Error\r\n"
				"Content-Type: text/plain\r\n"
				"Content-Length: 21\r\n"
				"\r\n"
				"Internal Server Error\r\n");
			case Type::503:
				return ("HTTP/1.1 503 Service Unavailable\r\n"
				"Retry-After: 120\r\n"
				"Content-Length: 0\r\n");
		}
		return {}; // fallback
	}
private:
	Type type_;
};
#endif
