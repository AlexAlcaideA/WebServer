#ifndef DEFAULTRESPONSE_HPP
# define DEFAULTRESPONSE_HPP
#include <string>
enum Type {
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
class DefaultResponse {
public:
	
	explicit DefaultResponse(){}

	std::string getString(Type type_) const {
		switch (type_) {
			case SUCCESS:
				return ("HTTP/1.1 200 OK\r\n"
				"Content-Type: text/plain\r\n"
				"Content-Length: 5\r\n"
				"\r\n"
				"Hello");
			case CREATED:
				return ("HTTP/1.1 201 Created\r\n"
				"Location: /users/123\r\n"
				"Content-Length: 0\r\n");
			case NO_CONTENT:
				return ("HTTP/1.1 204 No Content\r\n");
			case MOVED_PERMANENTLY:
				return ("HTTP/1.1 301 Moved Permanently\r\n"
				"Location: https://www.youtube.com/watch?v=dQw4w9WgXcQ\r\n"
				"Content-Length: 0\r\n");
			case FOUND:
				return ("HTTP/1.1 302 Found\r\n"
				"Location: /login\r\n"
				"Content-Length: 0\r\n");
			case NOT_MODIFIED:
				return ("HTTP/1.1 304 Not Modified\r\n"
				"ETag: \"Non modified\"\r\n");
			case BAD_REQUEST:
				return ("HTTP/1.1 400 Bad Request\r\n"
				"Content-Type: text/plain\r\n"
				"Content-Length: 11\r\n"
				"\r\n"
				"Bad Request\r\n");
			case UNAUTHORIZED:
				return ("HTTP/1.1 401 Unauthorized\r\n"
				"WWW-Authenticate: Basic realm=\"Example\"\r\n"
				"Content-Length: 0\r\n");
			case FORBIDDEN:
				return ("HTTP/1.1 403 Forbidden\r\n"
				"Content-Type: text/plain\r\n"
				"Content-Length: 11\r\n"
				"\r\n"
				"Forbidden\r\n");
			case NOT_FOUND:
				return ("HTTP/1.1 404 Not Found\r\n"
				"Content-Length: 13\r\n"
				"Content-Type: text/plain\r\n"
				"Connection: close\r\n"
				"\r\n"
				"404 Not Found\r\n");
			case METHOD_NOT_ALLOWED:
				return ("HTTP/1.1 405 Method Not Allowed\r\n"
				"Content-Length: 18\r\n"
				"Content-Type: text/plain\r\n"
				"Connection: close\r\n"
				"\r\n"
				"Method Not Allowed");
			case TEAPOT:
				return ("HTTP/1.1 418 I'm a teapot\r\n"
				"Content-Length: 14\r\n"
				"\r\n"
				"I'm a teapot\r\n");
			case INTERNAL_ERROR:
				return ("HTTP/1.1 500 Internal Server Error\r\n"
				"Content-Type: text/plain\r\n"
				"Content-Length: 21\r\n"
				"\r\n"
				"Internal Server Error\r\n");
			case SERVICE_DOWN:
				return ("HTTP/1.1 503 Service Unavailable\r\n"
				"Retry-After: 120\r\n"
				"Content-Length: 0\r\n");
		}
		return ("HTTP/1.1 500 Internal Server Error\r\n"
				"Content-Type: text/plain\r\n"
				"Content-Length: 21\r\n"
				"\r\n"
				"Internal Server Error\r\n");
	}
};
#endif
