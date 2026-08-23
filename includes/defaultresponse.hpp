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
            case Type::404:
                return ("HTTP/1.1 404 Not Found\r\n"
				"Content-Length: 13\r\n"
				"Content-Type: text/plain\r\n"
				"Connection: close\r\n"
				"\r\n"
				"404 Not Found");
            case Type::201:
                return ();
            case Type::204:
                return ();
            case Type::301:
                return ();
            case Type::302:
                return ();
            case Type::304:
                return ();
            case Type::400:
                return ();
            case Type::401:
                return ();
            case Type::403:
                return ();
            case Type::405:
                return ();
            case Type::418:
                return ();
            case Type::500:
                return ();
            case Type::503:
                return ();
        }

        return {}; // fallback
    }

private:
    Type type_;
};
#endif
