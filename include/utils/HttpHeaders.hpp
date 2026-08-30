#pragma once

#include <string>

namespace HttpHeaders
{
	enum Headers
	{
		HOST,
		USER_AGENT,
		ACCEPT,
		CONTENT_TYPE,
		UNKOWN_HEADER
	};

	Headers stringToHeaders(const std::string& str);
	std::string HeadersToString(Headers header);
};
