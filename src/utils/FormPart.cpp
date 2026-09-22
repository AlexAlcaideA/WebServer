#include "utils/FormPart.hpp"

namespace form
{
	std::vector<FormPart> parseMultipart(const std::string& body, const std::string& boundary)
	{
		std::vector<FormPart> parts;
		const std::string delimiter = "--" + boundary;
		const std::string endDelimiter = delimiter + "--";

		size_t pos = 0;
		while (pos < body.size())
		{
			size_t start = body.find(delimiter, pos);
			if (start == std::string::npos) break;
			if (body.compare(start, endDelimiter.size(), endDelimiter) == 0) break;

			start += delimiter.size();
			if (start + 2 <= body.size() && body.compare(start, 2, "\r\n") == 0)
				start += 2;

			size_t headersEnd = body.find("\r\n\r\n", start);
			if (headersEnd == std::string::npos) break;

			std::string headers = body.substr(start, headersEnd - start);
			size_t dataStart = headersEnd + 4;

			size_t next = body.find("\r\n" + delimiter, dataStart);
			if (next == std::string::npos) break;

			FormPart p;
			p.data = body.substr(dataStart, next - dataStart);
			p.name = extractQuotedValue(headers, "name");
			p.filename = extractQuotedValue(headers, "filename");
			p.contentType = extractPlainValue(headers, "Content-Type");
			parts.push_back(p);

			pos = next + 2;
		}
		return parts;
	}

	std::string extractQuotedValue(const std::string& headers, const std::string& key)
	{
		std::string search = key + "=\"";
		size_t pos = headers.find(search);
		if (pos == std::string::npos)
			return "";
		pos += search.size();
		size_t end = headers.find('"', pos);
		if (end == std::string::npos)
			return "";
		return headers.substr(pos, end - pos);
	}

	std::string extractPlainValue(const std::string& headers, const std::string& key)
	{
		std::string search = key + ": ";
		size_t pos = headers.find(search);
		if (pos == std::string::npos)
			return "";
		pos += search.size();
		size_t end = headers.find("\r\n", pos);
		if (end == std::string::npos)
			end = headers.size();
		return headers.substr(pos, end - pos);
	}

	std::string sanitizeFilename(const std::string& filename)
	{
		size_t slash = filename.find_last_of("/\\");
		std::string name = (slash != std::string::npos) ? filename.substr(slash + 1) : filename;

		if (name.empty() || name == "." || name == "..")
			return "";

		if (name.find("..") != std::string::npos)
			return "";

		return name;
	}

	std::string extractFilename(const std::string& disposition)
	{
		const std::string key = "filename=\"";
		size_t pos = disposition.find(key);
		if (pos == std::string::npos)
			return "";
		pos += key.size();
		size_t end = disposition.find('"', pos);
		if (end == std::string::npos)
			return "";
		return disposition.substr(pos, end - pos);
	}

}
