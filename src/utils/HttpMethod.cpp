#include "../../include/utils/HttpMethod.hpp"
#include <stdexcept>

namespace Http
{
	std::string methodToString(Http::Method method)
	{
		switch (method)
		{
			case Http::GET:
				return "GET";
			case Http::POST:
				return "POST";
			case Http::DELETE:
				return "DELETE";
			default:
				throw std::invalid_argument("Invalid method.");
		}
	}

	Http::Method stringToMethod(const std::string& str)
	{
		if (str == "GET")
			return Http::GET;
		if (str == "POST")
			return Http::POST;
		if (str == "DELETE")
			return Http::DELETE;
		return Http::UNKNOWN;
	}
}
