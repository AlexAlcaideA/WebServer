#include "../../include/utils/StringUtils.hpp"
#include <sstream>

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
}