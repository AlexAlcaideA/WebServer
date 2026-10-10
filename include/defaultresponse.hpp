#ifndef DEFAULT_RESPONSE_HPP
#define DEFAULT_RESPONSE_HPP

#include "includes.hpp"

enum Type
{
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
    LENGTH_REQUIRED = 411,
    TEAPOT = 418,
    INTERNAL_ERROR = 500,
    NOT_IMPLEMENTED = 501,
    SERVICE_DOWN = 503
};

class DefaultResponse
{
private:
    std::map<Type, std::string> responses;

public:
    DefaultResponse()
    {
        responses[SUCCESS] =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 5\r\n"
            "\r\n"
            "Hello";

        responses[CREATED] =
            "HTTP/1.1 201 Created\r\n"
            "Location: /users/123\r\n"
            "Content-Length: 0\r\n"
            "\r\n";

        responses[NO_CONTENT] =
            "HTTP/1.1 204 No Content\r\n"
            "Content-Length: 0\r\n"
            "\r\n";

        responses[MOVED_PERMANENTLY] =
            "HTTP/1.1 301 Moved Permanently\r\n"
            "Location: https://www.youtube.com/watch?v=dQw4w9WgXcQ\r\n"
            "Content-Length: 0\r\n"
            "\r\n";

        responses[FOUND] =
            "HTTP/1.1 302 Found\r\n"
            "Location: /login\r\n"
            "Content-Length: 0\r\n"
            "\r\n";

        responses[NOT_MODIFIED] =
            "HTTP/1.1 304 Not Modified\r\n"
            "ETag: \"Non modified\"\r\n"
            "Content-Length: 0\r\n"
            "\r\n";

        responses[BAD_REQUEST] =
            "HTTP/1.1 400 Bad Request\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 12\r\n"
            "\r\n"
            "Bad Request\r\n";

        responses[UNAUTHORIZED] =
            "HTTP/1.1 401 Unauthorized\r\n"
            "WWW-Authenticate: Basic realm=\"Example\"\r\n"
            "Content-Length: 0\r\n"
            "\r\n";

        responses[FORBIDDEN] =
            "HTTP/1.1 403 Forbidden\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 11\r\n"
            "\r\n"
            "Forbidden\r\n";

        responses[NOT_FOUND] =
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Length: 14\r\n"
            "Content-Type: text/plain\r\n"
            "Connection: close\r\n"
            "\r\n"
            "404 Not Found\r\n";

        responses[METHOD_NOT_ALLOWED] =
            "HTTP/1.1 405 Method Not Allowed\r\n"
            "Content-Length: 18\r\n"
            "Content-Type: text/plain\r\n"
            "Connection: close\r\n"
            "\r\n"
            "Method Not Allowed";

        responses[LENGTH_REQUIRED] =
            "HTTP/1.1 411 Length Required\r\n"
            "Content-Length: 0\r\n"
            "Connection: close\r\n"
            "\r\n";

        responses[TEAPOT] =
            "HTTP/1.1 418 I'm a teapot\r\n"
            "Content-Length: 14\r\n"
            "\r\n"
            "I'm a teapot\r\n";

        responses[INTERNAL_ERROR] =
            "HTTP/1.1 500 Internal Server Error\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 23\r\n"
            "\r\n"
            "Internal Server Error\r\n";

        responses[NOT_IMPLEMENTED] =
            "HTTP/1.1 501 Not Implemented\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 23\r\n"
            "\r\n"
            "Method not implemented\r\n";

        responses[SERVICE_DOWN] =
            "HTTP/1.1 503 Service Unavailable\r\n"
            "Retry-After: 120\r\n"
            "Content-Length: 0\r\n"
            "\r\n";
    }

    std::string getString(Type type) const
    {
        std::map<Type, std::string>::const_iterator it =
            responses.find(type);

        if (it != responses.end())
            return it->second;

        return responses.find(INTERNAL_ERROR)->second;
    }
};
#endif

