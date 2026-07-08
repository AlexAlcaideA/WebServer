#pragma once

#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <fstream>
#include "../utils/HttpMethod.hpp"

class ConfigContext
{
	protected:
		enum Method
		{
			GET,
			POST,
			DELETE
		};
		std::string* _root;									// No multiple,	No duplicates,	Optional,		Default: "html"
		std::vector<std::string>* _index;					// Multiple,	Duplicates,		Optional,		Default: index.html
		bool* _autoIndex;									// No Multiple,	No duplicates,	Optional,		Default: off
		std::string* _clientMaxBodySize;					// No multiple,	No duplicates,	Optional,		Default: 1m(mega)
		std::map<unsigned int, std::string>* _errorPage;	// Multiple,	Duplicates,		Optional,		Default: none
	public:
		ConfigContext();
		ConfigContext(const ConfigContext& other);
		ConfigContext& operator=(const ConfigContext& other);
		virtual ~ConfigContext();
		virtual std::ostream& operator<<(std::ostream& os) const;
		virtual void SetRoot(const std::string& path);
		virtual void AddIndex(const std::string& file);
		virtual void SetAutoIndex(bool on);
		virtual void SetClientMaxBodySize(const std::string& size);
		virtual void AddErrorPage(unsigned int code, const std::string& page);
		virtual void SetCgiHandler(const std::string&, const std::string& ) {}
};
