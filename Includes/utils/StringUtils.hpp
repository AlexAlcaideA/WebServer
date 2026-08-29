#pragma once

#include <string>

namespace utils
{
	bool stringToUnsignedInt(const std::string& s, unsigned int& result);
	bool stringToUnsignedLong(const std::string& s, unsigned long& result);
	bool stringToBytes(const std::string& s, unsigned long long& result);
	std::string unsignedLongLongToString(const unsigned long long& num);
	std::string unsignedLongToString(const unsigned long& num);
}
