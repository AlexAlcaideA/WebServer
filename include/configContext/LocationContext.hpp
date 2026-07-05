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
		std::vector<Method>* _limitExcept;					// No multiple,	No duplicates,	Optional,		Default: GET POST
		std::string* _uploadStore;							// No multiple,	No duplicates,	Optional,		Default: -
		ReturnVal* _returnVal;								// No multiple,	No duplicates,	Optional,		Default: -

	public:
		LocationContext();
		LocationContext(const LocationContext& other);
		LocationContext& operator=(const LocationContext& other);
		~LocationContext();
		void SetCgiHandler(const std::string& ext, const std::string& interp) override;
		void SetLimitExcept(const std::vector<Method>& methods);
		void SetUploadStore(const std::string& path);
		void SetReturn(unsigned int code, const std::string* url);
};
