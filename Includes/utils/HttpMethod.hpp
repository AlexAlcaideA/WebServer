#pragma once

#include <string>

namespace Http
{
	enum Method
	{
		GET,
		POST,
		DELETE,
	};

	std::string methodToString(Method method);
}