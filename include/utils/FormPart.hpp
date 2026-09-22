#pragma once

#include <vector>
#include <string>

namespace form
{
	struct FormPart
	{
		std::string name;
		std::string filename;
		std::string contentType;
		std::string data;
	};

	std::vector<FormPart> parseMultipart(const std::string& body, const std::string& boundary);
	std::string extractQuotedValue(const std::string& headers, const std::string& key);
	std::string extractPlainValue(const std::string& headers, const std::string& key);
	std::string sanitizeFilename(const std::string& filename);
	std::string extractFilename(const std::string& disposition);
}