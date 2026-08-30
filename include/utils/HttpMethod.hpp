#pragma once

#include <string>

namespace Http
{
	enum Method
	{
		GET,
		POST,
		DELETE,
		UNKNOWN
	};

	std::string methodToString(Method method);
	Method stringToMethod(const std::string& str);
}