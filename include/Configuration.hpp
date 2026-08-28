#pragma once

#include <string>
#include <vector>
#include <map>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include "configContext/GlobalContext.hpp"
#include "configContext/ServerContext.hpp"
#include "configContext/LocationContext.hpp"
#include "utils/HttpMethod.hpp"

class Configuration
{
	private:
		enum State
		{
			GLOBAL,
			SERVER,
			LOCATION
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
		void ProcessGlobalDirective(const std::vector<std::string>& args, GlobalContext* global);
		void ProcessServerDirective(const std::vector<std::string>& args, ServerContext* server);
		void ProcessLocationDirective(const std::vector<std::string>& args, LocationContext* location);
		bool Parse();
		Http::Method GetMethod(const std::string& arg);

	public:
		Configuration();
		Configuration(std::string confPath);
		Configuration(const Configuration& other);
		Configuration& operator=(const Configuration& other);
		~Configuration();

		const GlobalContext& GetConf() const;
};

std::ostream& operator<<(std::ostream& os, const Configuration& other);
