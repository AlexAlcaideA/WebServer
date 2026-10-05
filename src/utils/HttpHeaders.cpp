#include "../../include/utils/HttpHeaders.hpp"
#include <stdexcept>

namespace HttpHeaders
{
	HttpHeaders::Headers stringToHeaders(const std::string& str)
	{
		if (str == "Host")
			return HttpHeaders::HOST;
		if (str == "User-Agent")
			return HttpHeaders::USER_AGENT;
		if (str == "Accept")
			return HttpHeaders::ACCEPT;
		if (str == "Content-Type")
			return HttpHeaders::CONTENT_TYPE;
		return HttpHeaders::UNKOWN_HEADER;
	}

	std::string HeadersToString(HttpHeaders::Headers header)
	{
		switch (header)
		{
			case HttpHeaders::HOST:
				return "Host";
			case HttpHeaders::USER_AGENT:
				return "User-Agent";
			case HttpHeaders::ACCEPT:
				return "Accept";
			case HttpHeaders::CONTENT_TYPE:
				return "Content-Type";
			default:
				throw std::invalid_argument("Invalid header.");
		}
	}

	std::string extractPathFromUrl(const std::string& url)
	{

		size_t schemeEnd = url.find("://");
		if (schemeEnd == std::string::npos)
			return url.empty() ? "/" : url;

		size_t pathStart = url.find('/', schemeEnd + 3);
		if (pathStart == std::string::npos)
			return "/";

		return url.substr(pathStart);
	}

}
