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
		unsigned long long* _clientMaxBodySize;				// No multiple,	No duplicates,	Optional,		Default: 1m(mega)
		std::map<unsigned int, std::string>* _errorPage;	// Multiple,	Duplicates,		Optional,		Default: none
		std::map<std::string, std::string>* _cgiHandler;	// Multiple,	No Duplicates,	Optional,		Default: -
	public:
		ConfigContext();
		ConfigContext(const ConfigContext& other);
		ConfigContext& operator=(const ConfigContext& other);
		virtual ~ConfigContext();
		virtual void SetRoot(const std::string& path);
		virtual void AddIndex(const std::string& file);
		virtual void SetAutoIndex(bool on);
		virtual void SetClientMaxBodySize(const unsigned long long& size);
		virtual void AddErrorPage(unsigned int code, const std::string& page);
		virtual void SetCgiHandler(const std::string&, const std::string& ) {}

		virtual void print(std::ostream& os) const;

		virtual const std::string* GetRoot() const;
		virtual const std::vector<std::string>* GetIndexes() const;
		virtual const std::string* GetIndex(size_t index) const;
		virtual const bool* GetAutoIndex() const;
		virtual const unsigned long long* GetClientMaxBodySize() const;
		virtual const std::map<unsigned int, std::string>* GetErrorPages() const;
		virtual const std::string* GetErrorPage(unsigned int error) const;
		virtual const std::pair<const unsigned int, std::string>* GetErrorPageIndex(unsigned int index) const;
		virtual const std::string* GetCgiHandler(const std::string& extension) const;
		virtual const std::pair<const std::string, std::string>* GetCgiHandlerIndex(unsigned int index) const;
};

std::ostream& operator<<(std::ostream& os, const ConfigContext& other);
