#pragma once

#include "ConfigContext.hpp"

class LocationContext : public ConfigContext
{
	private:
		struct ReturnVal
		{
			unsigned int code;
			std::string* url; // Is optional, can be NULL
		};
		std::map<std::string, std::string>* _cgiHandlers;	// Multiple,	No duplicates,	Optional,		Default: -
		std::vector<Http::Method>* _limitExcept;			// No multiple,	No duplicates,	Optional,		Default: GET POST
		std::string* _uploadStore;							// No multiple,	No duplicates,	Optional,		Default: -
		ReturnVal* _returnVal;								// No multiple,	No duplicates,	Optional,		Default: -
		std::string _path;

	public:
		LocationContext();
		LocationContext(const LocationContext& other);
		LocationContext& operator=(const LocationContext& other);
		~LocationContext();
		bool operator<(const LocationContext& other) const;
    	bool operator==(const LocationContext& other) const;

		void SetCgiHandler(const std::string& ext, const std::string& interp);
		void SetLimitExcept(const std::vector<Http::Method>& methods);
		void SetUploadStore(const std::string& path);
		void SetReturn(unsigned int code, const std::string* url);
		void SetPath(const std::string& path);
		const std::string& GetPath() const;
};
