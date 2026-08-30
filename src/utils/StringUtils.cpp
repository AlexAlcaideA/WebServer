#include "../../include/utils/StringUtils.hpp"
#include <sstream>
#include <cmath>
#include <cerrno>

namespace utils
{
	bool stringToUnsignedInt(const std::string& s, unsigned int& result)
	{
		std::istringstream iss(s);
		unsigned int temp;
		if (!(iss >> temp))
			return false;
		char leftover;
		if (iss >> leftover)
			return false;
		result = temp;
		return true;
	}

	bool stringToUnsignedLong(const std::string& s, unsigned long& result)
	{
		if (s.empty())
			return false;

		errno = 0;
		char* end = NULL;
		unsigned long value = strtoul(s.c_str(), &end, 10);
		if (errno == ERANGE)
			return false;
		if (end == s.c_str())
			return false; // no number
		if (*end != '\0')
			return false; // characters after the number

		result = value;
		return true;
	}

	bool stringToBytes(const std::string& s, unsigned long long& result)
	{
		if (s.empty())
			return false;

		char* end = NULL;
		unsigned long long value = strtoull(s.c_str(), &end, 10);
		if (end == s.c_str())
			return false; // no number

		size_t multiplier = 1;
		if (*end != '\0')
		{
			switch (tolower(*end))
			{
				case 'k':
					multiplier = 1024;
					break;
				case 'm':
					multiplier = std::pow(1024, 2);
					break;
				case 'g':
					multiplier = std::pow(1024, 3);
					break;
				default:
					return false; // invalid
			}
			// No more chars after sign
			if (*(end + 1) != '\0')
				return false;
		}
		// Check overflow
		if (value > static_cast<unsigned long long>(-1) / multiplier)
			return false;

		result = static_cast<size_t>(value * multiplier);
		return true;
	}

	std::string unsignedLongLongToString(const unsigned long long& num)
	{
		std::ostringstream oss;
		oss << num;
		std::string str = oss.str();
		return str;
	}

	std::string unsignedLongToString(const unsigned long& num)
	{
		std::ostringstream oss;
		oss << num;
		std::string str = oss.str();
		return str;
	}

}
