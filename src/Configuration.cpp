#include "../include/Configuration.hpp"
#include <stack>
#include "../include/utils/StringUtils.hpp"

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
		// Decide si continuar o relanzar
	}
}

void Configuration::ProcessGlobalDirective(const std::vector<std::string>& args,
                                           GlobalContext* global)
{
	const std::string& directive = args[0];
	if (directive == "server")
	{
		std::cerr << "Error: 'server' debe ser un bloque (server { ... })" << std::endl;
		return;
	}
	// Directivas comunes que puede tener GlobalContext (root, index, error_page, etc.)
	// Se invocan mediante polimorfismo: global->SetRoot(...)
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
		global->SetClientMaxBodySize(args[1]);
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
	// No hay directivas específicas de global más allá de las comunes
}

void Configuration::ProcessServerDirective(const std::vector<std::string>& args,
    ServerContext* server)
{
	const std::string& directive = args[0];

	// Directivas comunes (también puede usarlas un servidor)
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
		server->SetClientMaxBodySize(args[1]);
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
	// Directivas específicas de servidor
	else if (directive == "listen")
	{
		if (args.size() < 2)
			throw std::invalid_argument("listen requires port");
		unsigned int port;
		// El último elemento es el puerto
		if (!utils::stringToUnsignedInt(args[args.size()-1], port))
			throw std::invalid_argument("invalid port: " + args[args.size()-1]);
		if (args.size() == 2) // solo puerto
			server->SetListen(port);
		else if (args.size() == 3) // IP y puerto
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
	// La adición de ubicaciones se hace desde el parser al encontrar un bloque location,
	// no mediante esta función.
}

void Configuration::ProcessLocationDirective(const std::vector<std::string>& args,
                                             LocationContext* location)
{
    const std::string& directive = args[0];

    // Directivas comunes
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
        location->SetClientMaxBodySize(args[1]);
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
    // Directivas específicas de ubicación
    else if (directive == "limit_except")
	{
        std::vector<Http::Method> methods;
        for (size_t i = 1; i < args.size(); ++i)
		{
            Http::Method m = GetMethod(args[i]);
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
            urlPtr = &urlStr; // urlStr vive durante la llamada, seguro
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
	std::vector<std::string> args;  // argumentos de la directiva actual
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
				// Procesar según el contexto actual
				ProcessDirective(args, stateStack.back(), global, currentServer, currentLocation);
				args.clear();
			}
		}
		else if (tok.type == Token::OPEN_BRACE)
		{
			// El token anterior debió indicar el contexto: "server" o "location"
			if (args.empty())
			{
				/* error */ return false;
			}
			std::string blockType = args[0];   // primer token: "server" o "location"

			if (blockType == "server")
			{
				if (stateStack.back() != GLOBAL)
				{
					std::cerr << "Error: bloque 'server' solo permitido en contexto global" << std::endl;
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
					std::cerr << "Error: bloque 'location' solo permitido dentro de un servidor" << std::endl;
					return false;
				}
				if (args.size() < 2)
				{
					/* error: falta el path */
					return false;
				}

				if (!currentServer)
				{
					/* error */
					return false;
				}

				LocationContext newLoc(args[1]);

				currentServer->AddLocation(newLoc);
				currentLocation = currentServer->GetLastLocation();
				stateStack.push_back(LOCATION);
			}
			else
			{
				std::cerr << "Error: bloque desconocido '" << blockType << "'" << std::endl;
				return false;
			}
			args.clear();
		}
		else if (tok.type == Token::CLOSE_BRACE)
		{
			if (stateStack.size() <= 1)
				/* error */ return false;
			stateStack.pop_back();
			// Actualizar punteros según el nuevo estado
			if (stateStack.back() == SERVER)
			{
				currentLocation = NULL;
				// currentServer sigue siendo el mismo
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
		std::cerr << "Error: llaves desbalanceadas (faltan cierres de bloque)" << std::endl;
		return false;
	}
	return true;
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
		throw std::exception();
	Parse();
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

std::ostream& Configuration::operator<<(std::ostream& os) const
{
	_globalBlock << os;
	return os;
}
