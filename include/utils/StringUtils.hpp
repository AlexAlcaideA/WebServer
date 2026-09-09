#pragma once

#include <string>
#include <stdint.h>

namespace utils
{
	bool stringToUnsignedInt(const std::string& s, unsigned int& result);
	bool stringToUnsignedLong(const std::string& s, unsigned long& result);
	bool stringToBytes(const std::string& s, unsigned long long& result);
	std::string fileToString(const std::string& fileName);
	std::string unsignedLongLongToString(const unsigned long long& num);
	std::string unsignedLongToString(const unsigned long& num);
	std::string ipToString(const uint32_t& ipAddr);
	std::string extractHostname(const std::string& hostHeader);
	std::string joinPath(const std::string& a, const std::string& b);
	bool fileExists(const std::string& path);
	std::string stripQuotes(const std::string& str);
}
