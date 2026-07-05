#pragma once

#include <string>
#include <vector>
#include <map>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include "Parser.hpp"

class Configuration
{
	public:
		enum Method
		{
			GET,
			POST,
			DELETE
		};
	private:
		enum State 
		{
			GLOBAL,
			SERVER,
			LOCATION
		};
		struct ReturnVal
		{
			unsigned int code;
			std::string* url; // Is optional, can be NULL
		};
		struct ServerListen
		{
			std::string serverIp;
			unsigned int port;
		};
		struct CgiHandler
		{
			std::string extension;
			std::string interpreter;
		};
		struct LocationContext
		{
			std::string* root;									// No multiple,	No duplicates,	Optional,		Default: "html"
			bool* autoIndex;									// No Multiple,	No duplicates,	Optional,		Default: off
			std::string* clientMaxBodySize;						// No multiple,	No duplicates,	Optional,		Default: 1m(mega)
			std::vector<std::string>* index;					// Multiple,	Duplicates,		Optional,		Default: index.html
			std::map<unsigned int, std::string>* errorPage;		// Multiple,	Duplicates,		Optional,		Default: none
			CgiHandler* cgiHandler;								// Multiple,	No duplicates,	Optional,		Default: -
			std::vector<Method>* limitExcept;					// No multiple,	No duplicates,	Optional,		Default: GET POST
			std::string* uploadStore;							// No multiple,	No duplicates,	Optional,		Default: -
			ReturnVal* returnVal;								// No multiple,	No duplicates,	Optional,		Default: -
			LocationContext();
			~LocationContext();
		};
		struct ServerContext
		{
			ServerListen* listen;								// Multiple,	No duplicates,	Optional,		Default: INADDR_ANY:http
			std::string* serverName;							// Multiple,	No Duplicates,	Optional,		Default: ""
			std::string* root;									// No multiple,	No duplicates,	Optional,		Default: "html"
			std::vector<std::string>* index;					// Multiple,	Duplicates,		Optional,		Default: index.html
			std::map<unsigned int, std::string>* errorPage;		// Multiple,	Duplicates,		Optional,		Default: none
			std::vector<LocationContext>* location;				// Multiple,	No duplicates,	Optional,		Default: /
			bool* autoIndex;									// No Multiple,	No duplicates,	Optional,		Default: off
			std::string* clientMaxBodySize;						// No multiple,	No duplicates,	Optional,		Default: 1m(mega)
			CgiHandler* cgiHandler;								// Multiple,	No duplicates,	Optional,		Default: -
			ServerContext();
			~ServerContext();
		};
		struct GlobalContext
		{
			std::vector<ServerContext> server;					// Multiple,	Duplicates,		No optional,	Default: -
			std::string* root;									// No multiple,	No duplicates,	Optional,		Default: "html"
			std::vector<std::string>* index;					// Multiple,	Duplicates,		Optional,		Default: index.html
			std::map<unsigned int, std::string>* errorPage;		// Multiple,	Duplicates,		Optional,		Default: none
			bool* autoIndex;									// No Multiple,	No duplicates,	Optional,		Default: off
			std::string* clientMaxBodySize;						// No multiple,	No duplicates,	Optional,		Default: 1m(mega)
			GlobalContext();
			~GlobalContext();
		};
		struct Token 
		{
			enum Type { WORD, OPEN_BRACE, CLOSE_BRACE, SEMICOLON, END };
			Type type;
			std::string value;
			Token();
			Token(Type t, std::string val);
		};

		GlobalContext _globalBlock;
		std::string _confPath;

		bool FileExistAndReadable(const std::string& path);
		bool ReadAndPreprocess(const std::string& path, std::string& content);
		Token GetNextToken(const std::string& src, size_t& pos);
		void ProcessDirective(const std::vector<std::string>& args, State ctx,
			GlobalContext* global, ServerContext* server, LocationContext* location);
		bool Parse();

	public:
		Configuration();
		Configuration(std::string confPath);
		Configuration(const Configuration& other);
		Configuration& operator=(const Configuration& other);
		~Configuration();
};