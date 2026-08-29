#include "../../Includes/configuration/Configuration.hpp"
#include <cstddef>
#include <exception>
#include <stack>
#include "../../Includes/utils/StringUtils.hpp"

Configuration::Token::Token()
{}

Configuration::Token::Token(Type t, std::string val) : type(t), value(val)
{}

bool Configuration::FileExistAndReadable(const std::string& path)
{
	return access(path.c_str(), R_OK) == 0;
}

bool Configuration::ReadAndPreprocess(const std::string& path, std::string& content)
{
	std::ifstream file(path.c_str());
	if (!file || !file.is_open())
		return false;

	std::string line;
	content.clear();
	while (std::getline(file, line))
	{
		size_t comment = line.find('#');
		if (comment != std::string::npos)
			line = line.substr(0, comment);

		size_t start = line.find_first_not_of(" \t\r\n");
		if (start == std::string::npos)
			continue;
		size_t end = line.find_last_not_of(" \t\r\n");
		line = line.substr(start, end - start + 1);
		if (!line.empty())
			content += line + "\n";
	}
	file.close();
	return true;
}

Configuration::Token Configuration::GetNextToken(const std::string& src, size_t& pos)
{
	while (pos < src.size() && isspace(src[pos]))
		pos++;
	if (pos >= src.size())
		return Configuration::Token(Token::END, "");

	char c = src[pos];
	if (c == '{')
	{
		++pos;
		return Configuration::Token(Token::OPEN_BRACE, "{");
	}
	if (c == '}')
	{
		++pos;
		return Configuration::Token(Token::CLOSE_BRACE, "}");
	}
	if (c == ';')
	{
		++pos;
		return Configuration::Token(Token::SEMICOLON, ";");
	}

	size_t start = pos;
	while (pos < src.size() && !isspace(src[pos]) && src[pos] != '{' && src[pos] != '}' && src[pos] != ';')
		pos++;
	return Token(Token::WORD, src.substr(start, pos - start));
}

void Configuration::ProcessDirective(const std::vector<std::string>& args,
	State ctx, GlobalContext* global, ServerContext* server, LocationContext* location)
{
	if (args.empty())
		return;
	try
	{
		switch (ctx)
		{
			case GLOBAL:
				if (global)
					ProcessGlobalDirective(args, global);
				break;
			case SERVER:
				if (server)
					ProcessServerDirective(args, server);
				break;
			case LOCATION:
				if (location)
					ProcessLocationDirective(args, location);
				break;
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Config error: " << e.what() << std::endl;
		// TEMP Decide si continuar o relanzar
	}
}

void Configuration::ProcessGlobalDirective(const std::vector<std::string>& args,
                                           GlobalContext* global)
{
	const std::string& directive = args[0];
	if (directive == "server")
	{
		std::cerr << "Error: 'server' must be a block (server { ... })" << std::endl;
		return;
	}
	// Common directives
	if (directive == "root")
	{
		if (args.size() < 2)
			throw std::invalid_argument("root requires a path");
		global->SetRoot(args[1]);
	}
	else if (directive == "index")
	{
		for (size_t i = 1; i < args.size(); ++i)
			global->AddIndex(args[i]);
	}
	else if (directive == "autoindex")
	{
		if (args.size() < 2)
			throw std::invalid_argument("autoindex requires on/off");
		global->SetAutoIndex(args[1] == "on");
	}
	else if (directive == "client_max_body_size")
	{
		if (args.size() < 2)
			throw std::invalid_argument("client_max_body_size requires a size");
		unsigned long long num;
		utils::stringToBytes(args[1], num) ? global->SetClientMaxBodySize(num) : global->SetClientMaxBodySize(1024 * 1024);
	}
	else if (directive == "error_page")
	{
		if (args.size() < 3)
			throw std::invalid_argument("error_page requires code and page");
		unsigned int code;
		if (!utils::stringToUnsignedInt(args[1], code))
			throw std::invalid_argument("invalid error code: " + args[1]);
		global->AddErrorPage(code, args[2]);
	}
}

void Configuration::ProcessServerDirective(const std::vector<std::string>& args,
    ServerContext* server)
{
	const std::string& directive = args[0];

	// Common directives
	if (directive == "root")
	{
		if (args.size() < 2)
			throw std::invalid_argument("root requires a path");
		server->SetRoot(args[1]);
	}
	else if (directive == "index")
	{
		for (size_t i = 1; i < args.size(); ++i)
			server->AddIndex(args[i]);
	}
	else if (directive == "autoindex")
	{
		if (args.size() < 2)
			throw std::invalid_argument("autoindex requires on/off");
		server->SetAutoIndex(args[1] == "on");
	}
	else if (directive == "client_max_body_size")
	{
		if (args.size() < 2)
			throw std::invalid_argument("client_max_body_size requires a size");
		unsigned long long num;
		utils::stringToBytes(args[1], num) ? server->SetClientMaxBodySize(num) : server->SetClientMaxBodySize(1024 * 1024);
	}
	else if (directive == "error_page")
	{
		if (args.size() < 3)
			throw std::invalid_argument("error_page requires code and page");
		unsigned int code;
		if (!utils::stringToUnsignedInt(args[1], code))
			throw std::invalid_argument("invalid error code: " + args[1]);
		server->AddErrorPage(code, args[2]);
	}
	// Directives from the server context
	else if (directive == "listen")
	{
		if (args.size() < 2)
			throw std::invalid_argument("listen requires port");
		unsigned int port;
		// The last element is the port
		if (!utils::stringToUnsignedInt(args[args.size()-1], port))
			throw std::invalid_argument("invalid port: " + args[args.size()-1]);
		if (args.size() == 2) // only port
			server->SetListen(port);
		else if (args.size() == 3) // IP and port
			server->SetListen(args[1], port);
		else
			throw std::invalid_argument("listen takes 1 or 2 arguments");
	}
	else if (directive == "server_name")
	{
		for (size_t i = 1; i < args.size(); ++i)
			server->AddServerName(args[i]);
	}
	else if (directive == "cgi_handler")
	{
		if (args.size() != 3)
			throw std::invalid_argument("cgi_handler requires extension and interpreter");
		server->SetCgiHandler(args[1], args[2]);
	}
	else if (directive == "location")
		std::cerr << "Error: 'location' debe ser un bloque (location ... { ... })" << std::endl;
	// The addition of locations is done from the parser when encountering a location block,
	// not through this function.
}

void Configuration::ProcessLocationDirective(const std::vector<std::string>& args,
                                             LocationContext* location)
{
    const std::string& directive = args[0];

    // Common directives
    if (directive == "root")
	{
        if (args.size() < 2)
			throw std::invalid_argument("root requires a path");
        location->SetRoot(args[1]);
    }
    else if (directive == "index")
	{
        for (size_t i = 1; i < args.size(); ++i)
            location->AddIndex(args[i]);
    }
    else if (directive == "autoindex")
	{
        if (args.size() < 2)
			throw std::invalid_argument("autoindex requires on/off");
        location->SetAutoIndex(args[1] == "on");
    }
    else if (directive == "client_max_body_size")
	{
        if (args.size() < 2)
			throw std::invalid_argument("client_max_body_size requires a size");
        unsigned long long num;
		utils::stringToBytes(args[1], num) ? location->SetClientMaxBodySize(num) : location->SetClientMaxBodySize(1024 * 1024);
    }
    else if (directive == "error_page")
	{
        if (args.size() < 3)
			throw std::invalid_argument("error_page requires code and page");
        unsigned int code;
        if (!utils::stringToUnsignedInt(args[1], code))
            throw std::invalid_argument("invalid error code: " + args[1]);
        location->AddErrorPage(code, args[2]);
    }
    // Location context directives
    else if (directive == "limit_except")
	{
        std::vector<Http::Method> methods;
        for (size_t i = 1; i < args.size(); ++i)
		{
			Http::Method m;
			try
			{
				m = GetMethod(args[i]);
			}
            catch(...)
        	{
         		throw std::invalid_argument("Couldn't find method; " + args[i]);
         	}
            methods.push_back(m);
        }
        location->SetLimitExcept(methods);
    }
    else if (directive == "upload_store")
	{
        if (args.size() < 2)
			throw std::invalid_argument("upload_store requires a path");
        location->SetUploadStore(args[1]);
    }
    else if (directive == "return")
	{
        if (args.size() < 2)
			throw std::invalid_argument("return requires a code");
        unsigned int code;
        if (!utils::stringToUnsignedInt(args[1], code))
            throw std::invalid_argument("invalid return code: " + args[1]);
        const std::string* urlPtr = NULL;
        std::string urlStr;
        if (args.size() >= 3)
		{
            urlStr = args[2];
            urlPtr = &urlStr;
        }
        location->SetReturn(code, urlPtr);
    }
    else if (directive == "cgi_handler")
	{
        if (args.size() != 3)
			throw std::invalid_argument("cgi_handler requires extension and interpreter");
        location->SetCgiHandler(args[1], args[2]);
    }
}

bool Configuration::Parse()
{
	std::string content;
	if (!ReadAndPreprocess(_confPath, content))
	{
		std::cerr << "Cannot open file" << std::endl;
		return false;
	}

	size_t pos = 0;
	std::vector<std::string> args;  // args from the current directive
	std::vector<State> stateStack;
	stateStack.push_back(GLOBAL);

	GlobalContext* global = &_globalBlock;
	ServerContext* currentServer = NULL;
	LocationContext* currentLocation = NULL;

	Configuration::Token tok;
	while ((tok = GetNextToken(content, pos)).type != Token::END)
	{
		if (tok.type == Token::WORD)
			args.push_back(tok.value);
		else if (tok.type == Token::SEMICOLON)
		{
			if (args.empty())
			{
				/* error: directiva vacía */
				throw std::exception();
			}
			if (args[0] == "server" || args[0] == "location")
			{
				std::cerr << "Error: '" << args[0] << "' debe ser un bloque, no una directiva terminada en ';'" << std::endl;
				return false;
			}
			else
			{
				// Process according the current context
				try
				{
					ProcessDirective(args, stateStack.back(), global, currentServer, currentLocation);
				}
				catch(const std::exception& e)
				{
					std::cerr << e.what() << std::endl;
					return false;
				}
				args.clear();
			}
		}
		else if (tok.type == Token::OPEN_BRACE)
		{
			// he previous token should have indicated the context: "server" or "location"
			if (args.empty())
				return false;
			std::string blockType = args[0];   // first token: "server" or "location"

			if (blockType == "server")
			{
				if (stateStack.back() != GLOBAL)
				{
					std::cerr << "Error: block 'server' only avaliable in global context" << std::endl;
					return false;
				}
				ServerContext newServer;
				global->AddServer(newServer);
				currentServer = global->GetLastServer();
				currentLocation = NULL;
				stateStack.push_back(SERVER);
			}
			else if (blockType == "location")
			{
				if (stateStack.back() != SERVER)
				{
					std::cerr << "Error: block 'location' only avaliable inside a server" << std::endl;
					return false;
				}
				if (args.size() < 2)
				{
					std::cerr << "Error: No path" << std::endl;
					return false;
				}
				if (!currentServer)
					return false;

				LocationContext newLoc(args[1]);

				currentServer->AddLocation(newLoc);
				currentLocation = currentServer->GetLastLocation();
				stateStack.push_back(LOCATION);
			}
			else
			{
				std::cerr << "Error: unkown block '" << blockType << "'" << std::endl;
				return false;
			}
			args.clear();
		}
		else if (tok.type == Token::CLOSE_BRACE)
		{
			if (stateStack.size() <= 1)
				/* error */ return false;
			stateStack.pop_back();
			// Update pointers for the current state
			if (stateStack.back() == SERVER)
			{
				currentLocation = NULL;
				// currentServer is still the same
			}
			else if
			(stateStack.back() == GLOBAL)
			{
				currentServer = NULL;
				currentLocation = NULL;
			}
		}
	}
	if (stateStack.size() != 1)
	{
		std::cerr << "Error: unbalance braces (missing closing braces)" << std::endl;
		return false;
	}
	return true;
}

void CheckLocation(const ServerContext& server, LocationContext& location)
{
	if (location.GetRoot() == NULL)
		location.SetRoot(*server.GetRoot());
	if (location.GetAutoIndex() == NULL)
		location.SetAutoIndex(server.GetAutoIndex());
	if (location.GetClientMaxBodySize() == NULL)
		location.SetClientMaxBodySize(*server.GetClientMaxBodySize());
	if (location.GetIndexes() == NULL)
	{
		for (size_t i = 0; i < server.GetIndexes()->size(); i++)
			location.AddIndex(*server.GetIndex(i));
	}
	if (location.GetErrorPages() == NULL)
	{
		if (server.GetErrorPages() != NULL)
		{
			for (size_t i = 0; i < server.GetErrorPages()->size(); i++)
			{
				std::pair<const unsigned int, std::string> values = *(server.GetErrorPageIndex(i));
				location.AddErrorPage(values.first, values.second);
			}
		}
	}
	if (location.GetCgiHandlers() == NULL)
	{
		if (server.GetCgiHandlers() != NULL)
		{
			for (size_t i = 0; i < server.GetCgiHandlers()->size(); i++)
			{
				std::pair<const std::string, std::string> values = *(server.GetCgiHandlerIndex(i));
				location.SetCgiHandler(values.first, values.second);
			}
		}
	}
	if (location.GetLimitExcepts() == NULL)
	{
		std::vector<Http::Method>methods;
		methods.push_back(Http::GET);
		methods.push_back(Http::POST);
		location.SetLimitExcept(methods);
	}
}

void CheckServer(const GlobalContext& global, ServerContext& server)
{
	if (server.GetListens() == NULL)
		server.SetListen(80);
	if (server.GetServerNames() == NULL)
		server.AddServerName("");
	if (server.GetRoot() == NULL)
		server.SetRoot(*global.GetRoot());
	if (server.GetIndexes() == NULL)
		server.AddIndex("index.html");
	if (server.GetErrorPages() == NULL)
	{
		if (global.GetErrorPages() != NULL)
		{
			for (size_t i = 0; i < global.GetErrorPages()->size(); i++)
			{
				std::pair<const unsigned int, std::string> values = *(global.GetErrorPageIndex(i));
				server.AddErrorPage(values.first, values.second);
			}
		}
	}
	if (server.GetAutoIndex() == NULL)
		server.SetAutoIndex(global.GetAutoIndex());
	if (server.GetClientMaxBodySize() == NULL)
		server.SetClientMaxBodySize(*global.GetClientMaxBodySize());
	if (server.GetLocations() == NULL)
	{
		LocationContext tmpLocation;
		tmpLocation.SetRoot(*server.GetRoot());
		tmpLocation.SetAutoIndex(server.GetAutoIndex());
		tmpLocation.SetClientMaxBodySize(*server.GetClientMaxBodySize());
		for (size_t i = 0; i < server.GetIndexes()->size(); i++)
			tmpLocation.AddIndex(*server.GetIndex(i));
		for (size_t i = 0; i < server.GetErrorPages()->size(); i++)
		{
			std::pair<const unsigned int, std::string> values = *(server.GetErrorPageIndex(i));
			tmpLocation.AddErrorPage(values.first, values.second);
		}
		if (server.GetCgiHandlers() != NULL)
		{
			for (size_t i = 0; i < server.GetCgiHandlers()->size(); i++)
			{
				std::pair<const std::string, std::string> values = *(server.GetCgiHandlerIndex(i));
				tmpLocation.SetCgiHandler(values.first, values.second);
			}
		}
		std::vector<Http::Method>methods;
		methods.push_back(Http::GET);
		methods.push_back(Http::POST);
		tmpLocation.SetLimitExcept(methods);
		server.AddLocation(tmpLocation);
	}
	else
	{
		for (size_t i = 0; i < server.GetLocations()->size(); i++)
			CheckLocation(server, server.GetLocation(i));
	}
}

void CheckGlobal(GlobalContext& global)
{
	if (global.GetRoot() == NULL)
		global.SetRoot("html");
	if (global.GetIndexes() == NULL)
		global.AddIndex("index.html");
	if (global.GetAutoIndex() == NULL)
		global.SetAutoIndex(false);
	if (global.GetClientMaxBodySize() == NULL)
	{
		unsigned long long num;
		utils::stringToBytes("1m", num);
		global.SetClientMaxBodySize(num);
	}
	for (size_t i = 0; i < global.GetServers()->size(); i++)
		CheckServer(global, global.GetServer(i));
}



void Configuration::FillWithDefaultVals()
{
	CheckGlobal(_globalBlock);
}

Http::Method Configuration::GetMethod(const std::string& arg)
{
	if (arg == "GET")
		return Http::GET;
	if (arg == "POST")
		return Http::POST;
	if (arg == "DELETE")
		return Http::DELETE;
	throw std::invalid_argument("Method couldn't be found");
}

Configuration::Configuration(std::string confPath) : _confPath(confPath)
{

	if (!FileExistAndReadable(confPath))
		throw std::invalid_argument("Invalid config path.");
	if (!Parse())
		throw std::invalid_argument("Invalid values in config file.");
	FillWithDefaultVals();
}

Configuration::Configuration(const Configuration& other)
{
	*this = other;
}

Configuration& Configuration::operator=(const Configuration& other)
{
	if (this != &other)
	{
		_globalBlock = other._globalBlock;
		_confPath = other._confPath;
	}
	return *this;
}

Configuration::~Configuration()
{}

const GlobalContext& Configuration::GetConf() const
{
	return _globalBlock;
}

std::ostream& operator<<(std::ostream& os, const Configuration& other)
{
	os << other.GetConf();
	return os;
}
