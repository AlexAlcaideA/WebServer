#include "../../include/utils/HttpStatus.hpp"

namespace HttpStatus
{
	std::string reasonPhrase(HttpStatus::HttpStatusCode code)
	{
		switch (code)
		{
			case 200: return "OK";
			case 201: return "Created";
			case 202: return "Accepted";
			case 204: return "No Content";
			case 301: return "Moved Permanently";
			case 302: return "Found";
			case 304: return "Not Modified";
			case 400: return "Bad Request";
			case 401: return "Unauthorized";
			case 403: return "Forbidden";
			case 404: return "Not Found";
			case 405: return "Method Not Allowed";
			case 411: return "Lenght Required";
			case 418: return "I'm a teapot";
			case 500: return "Internal Server Error";
			case 501: return "Not Implemented";
			case 503: return "Service Unvavailable";
			default:  return "Unknown Status";
		}
	}

	std::string reasonPhrase(size_t code)
	{
		switch (code)
		{
			case 200: return "OK";
			case 201: return "Created";
			case 202: return "Accepted";
			case 204: return "No Content";
			case 301: return "Moved Permanently";
			case 302: return "Found";
			case 304: return "Not Modified";
			case 400: return "Bad Request";
			case 401: return "Unauthorized";
			case 403: return "Forbidden";
			case 404: return "Not Found";
			case 405: return "Method Not Allowed";
			case 411: return "Lenght Required";
			case 418: return "I'm a teapot";
			case 500: return "Internal Server Error";
			case 501: return "Not Implemented";
			case 503: return "Service Unvavailable";
			default:  return "Unknown Status";
		}
	}
}
