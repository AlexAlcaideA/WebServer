#pragma once

#include <string>

namespace utils
{
	bool stringToUnsignedInt(const std::string& s, unsigned int& result);
	bool stringToUnsignedLongLong(const std::string& s, unsigned long long& result);
	std::string unsignedLongLongToString(const unsigned long long& num);
}
