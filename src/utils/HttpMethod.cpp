#include "../../Includes/utils/HttpMethod.hpp"

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
			return "UNKNOWN";
    }
}
