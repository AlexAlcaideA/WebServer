#pragma once

#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <ostream>
#include "../utils/HttpMethod.hpp"

class ConfigContext
{
	protected:
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

		virtual const std::string* GetRoot();
		virtual const std::vector<std::string>* GetIndexes();
		virtual const std::string* GetIndex(size_t index);
		virtual const bool* GetAutoIndex();
		virtual const std::string* GetClientMaxBodySize();
		virtual const std::map<unsigned int, std::string>* GetErrorPages();
		virtual const std::string* GetErrorPage(unsigned int error);
		virtual const std::string* GetCgiHandler(const std::string& extension) const;
};
