#include "../../include/utils/StringUtils.hpp"
#include <sstream>
#include <cmath>
#include <cerrno>
#include <fstream>
#include <arpa/inet.h>
#include <sys/stat.h>

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

	std::string fileToString(const std::string& fileName)
	{
		std::ifstream file(fileName.c_str());
		if (!file.is_open())
			return "";
		std::ostringstream ss;
		ss << file.rdbuf();
		return ss.str();
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

	std::string ipToString(const uint32_t& ipAddr)
	{
		uint32_t ip_host = ntohl(ipAddr);
		std::ostringstream oss;
		oss << ((ip_host >> 24) & 0xFF) << '.'
			<< ((ip_host >> 16) & 0xFF) << '.'
			<< ((ip_host >> 8) & 0xFF) << '.'
			<< (ip_host & 0xFF);
		return oss.str();
	}

	std::string extractHostname(const std::string& hostHeader)
	{
		size_t colonPos = hostHeader.find(':');
		if (colonPos != std::string::npos)
			return hostHeader.substr(0, colonPos);
		return hostHeader;
	}

	std::string joinPath(const std::string& a, const std::string& b)
	{
		if (a.empty())
			return b;
		if (b.empty())
			return a;
		std::string result = a;
		if (result[result.size()-1] != '/')
			result += '/';
		if (b[0] == '/')
			result += b.substr(1);
		else
			result += b;
		return result;
	}

	bool fileExists(const std::string& path)
	{
		struct stat buffer;
		return (stat(path.c_str(), &buffer) == 0 && S_ISREG(buffer.st_mode));
	}

	std::string stripQuotes(const std::string& str)
	{
		if (str.size() >= 2)
		{
			char first = str[0];
			char last = str[str.size() - 1];
			if ((first == '"' && last == '"') || (first == '\'' && last == '\''))
				return str.substr(1, str.size() - 2);
    	}
    	return str;
	}

	std::string extractBoundary(const std::string& contentType)
	{
		const std::string key = "boundary=";
		size_t pos = contentType.find(key);
		if (pos == std::string::npos)
			return "";
		pos += key.size();

		if (pos < contentType.size() && contentType[pos] == '"')
		{
			++pos;
			size_t end = contentType.find('"', pos);
			if (end == std::string::npos) return "";
			return contentType.substr(pos, end - pos);
		}
		size_t end = contentType.find(';', pos);
		if (end == std::string::npos) end = contentType.size();
		return contentType.substr(pos, end - pos);
	}

}
