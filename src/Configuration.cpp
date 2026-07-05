#include "../include/Configuration.hpp"

Configuration::LocationContext::LocationContext()
	: root(NULL), autoIndex(NULL), clientMaxBodySize(NULL),
	index(NULL), cgiHandler(NULL), limitExcept(NULL),
	uploadStore(NULL), returnVal(NULL)
{}

Configuration::LocationContext::~LocationContext()
{
	delete root;
	delete autoIndex;
	delete clientMaxBodySize;
	delete index;
	delete cgiHandler;
	delete limitExcept;
	delete uploadStore;
	delete returnVal;
}

Configuration::ServerContext::ServerContext()
	: listen(NULL), serverName(NULL), root(NULL),
	index(NULL), errorPage(NULL), location(NULL),
	autoIndex(NULL), clientMaxBodySize(NULL), cgiHandler(NULL)
{}

Configuration::ServerContext::~ServerContext()
{
	delete listen;
	delete serverName;
	delete root;
	delete index;
	delete errorPage;
	delete location;
	delete autoIndex;
	delete clientMaxBodySize;
	delete cgiHandler;
}

Configuration::GlobalContext::GlobalContext()
	: root(NULL), index(NULL), errorPage(NULL),
	autoIndex(NULL), clientMaxBodySize(NULL)
{}

Configuration::GlobalContext::~GlobalContext()
{
	delete root;
	delete index;
	delete errorPage;
	delete autoIndex;
	delete clientMaxBodySize;
}

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
	std::string directive = args[0];
	// args contiene la directiva y sus argumentos

	// Contexto Global
	if (ctx == GLOBAL)
	{
		if (directive == "server")
		{
			/* ya manejado con '{' */
		}
		else if (directive == "root")
			SetRoot(global, std::vector<std::string>(args.begin()+1, args.end()));
		else if (directive == "index")
			AddIndex(global, std::vector<std::string>(args.begin()+1, args.end()));
		// ...
	}
	// Contexto Server
	else if (ctx == SERVER && server)
	{
		if (directive == "location") { /* ya manejado */ }
		else if (directive == "listen")
			SetListen(server, args[1], args[2]);
		else if (directive == "root") 
			SetRoot(server, std::vector<std::string>(args.begin()+1, args.end()));
		// ...
	}
	// Contexto Location
	else if (ctx == LOCATION && location)
	{
		if (directive == "root")
			SetRoot(location, std::vector<std::string>(args.begin()+1, args.end()));
		else if (directive == "limit_except") setLimitExcept(location, ...);
		// ...
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
			std::string ctx = args.back(); // último argumento antes de '{'
			if (ctx == "server")
			{
				// Añadir nuevo ServerContext al vector global
				ServerContext newServer;
				// Inicializar punteros a NULL (el constructor ya lo hace)
				global->server.push_back(newServer);
				currentServer = &(global->server.back());
				currentLocation = NULL;
				stateStack.push_back(SERVER);
			}
			else if (ctx == "location")
			{
				// El path es el penúltimo argumento (args.size() >= 2)
				if (args.size() < 2) { /* error */ return false; }
				std::string path = args[args.size()-2]; // "location /path {"
				// Añadir LocationContext al servidor actual
				if (!currentServer) { /* error */ return false; }
				if (currentServer->location == NULL)
					currentServer->location = new std::vector<LocationContext>();
				LocationContext newLoc;
				// newLoc.path = path; (necesitas un campo path)
				currentServer->location->push_back(newLoc);
				currentLocation = &(currentServer->location->back());
				stateStack.push_back(LOCATION);
			}
			else
			{
				// bloque desconocido
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
	return true;
}

Configuration::Configuration(std::string confPath) : _confPath(confPath)
{

	if (!FileExistAndRedeable(confPath))
		throw std::exception();
	std::string context;
	if (!ReadAndPreprocess(confPath, context))
		throw std::exception();
}
