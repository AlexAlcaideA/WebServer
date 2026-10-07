#include "cgi/CgiHandler.hpp"

bool CgiHandler::_buildEnv(const HttpRequest& req, const std::string& clientIp, const std::string& serverName, unsigned int serverPort)
{
	_env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	_env.push_back("SERVER_PROTOCOL=" + req.getHttpVersion());
	_env.push_back("REQUEST_METHOD=" + Http::methodToString(req.getMethod()));
	_env.push_back("SCRIPT_NAME=" + req.getRequestTarget());
	_env.push_back("SCRIPT_FILENAME=" + _scriptPath);
	_env.push_back("REMOTE_ADDR=" + clientIp);
	_env.push_back("SERVER_NAME=" + serverName);
	_env.push_back("SERVER_PORT=" + utils::unsignedLongToString(serverPort));
	_env.push_back("SERVER_SOFTWARE=webserv/1.0");
	_env.push_back("PATH=/usr/local/bin:/usr/bin:/bin");

	const std::string& target = req.getRequestTarget();
	size_t q = target.find('?');
	if (q != std::string::npos)
		_env.push_back("QUERY_STRING=" + target.substr(q + 1));
	else
		_env.push_back("QUERY_STRING=");
	
	const std::string* cl = req.getHeader("Content-Length");
	if (cl)
		_env.push_back("CONTENT_LENGTH=" + *cl);
	const std::string* ct = req.getHeader("Content-Type");
	if (ct)
		_env.push_back("CONTENT_TYPE=" + *ct);

	const std::map<std::string, std::string> headers = req.getHeaders();

	for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it)
	{
		std::string key = "HTTP_";
		for (size_t i = 0; i < it->first.size(); ++i)
		{
			char c = it->first[i];
			if (c == '-')
				key += '_';
			else
				key += toupper(c);
		}
		_env.push_back(key + "=" + it->second);
	}
	return true;
}

HttpResponse CgiHandler::_parseCgiOutput() const
{
	size_t headerEnd = _output.find("\r\n\r\n");
	if (headerEnd == std::string::npos)
		return HttpResponse(HTTP_VER, std::map<std::string, std::string>(), 500);

	std::string headersBlock = _output.substr(0, headerEnd);
	std::string body = _output.substr(headerEnd + 4);

	std::map<std::string, std::string> headers;
	size_t statusCode = 200;
	std::string reason = "OK";

	std::istringstream iss(headersBlock);
	std::string line;
	while (std::getline(iss, line))
	{
		if (!line.empty() && line[line.size()-1] == '\r')
			line.erase(line.size()-1);

		size_t colon = line.find(':');
		if (colon == std::string::npos)
			continue;

		std::string key = line.substr(0, colon);
		std::string value = line.substr(colon + 1);
		size_t first = value.find_first_not_of(" \t");
		if (first != std::string::npos)
			value = value.substr(first);

		if (key == "Status")
		{
			std::istringstream st(value);
			st >> statusCode;
			std::getline(st, reason);
			if (!reason.empty() && reason[0] == ' ')
				reason = reason.substr(1);
		}
		else
			headers[key] = value;
	}

	// if CGI doesn't have lenght
	if (headers.find("Content-Length") == headers.end())
		headers["Content-Length"] = utils::unsignedLongToString(body.size());

	return HttpResponse(HTTP_VER, headers, body.size(), body, statusCode);
}

std::string CgiHandler::_dechunk(const std::string& body)
{
	std::string result;
	size_t pos = 0;

	while (pos < body.size())
	{
		size_t lineEnd = body.find("\r\n", pos);
		if (lineEnd == std::string::npos)
			break;

		std::string sizeStr = body.substr(pos, lineEnd - pos);

		size_t semi = sizeStr.find(';');
		if (semi != std::string::npos)
			sizeStr = sizeStr.substr(0, semi);

		std::istringstream iss(sizeStr);
		size_t chunkSize = 0;
		iss >> std::hex >> chunkSize;

		if (chunkSize == 0)
			break; // last chunk

		pos = lineEnd + 2; // skip \r\n

		// Copy data
		if (pos + chunkSize > body.size())
			break;
		result.append(body, pos, chunkSize);

		pos += chunkSize + 2; // skip chunk + \r\n
	}
	return result;
}

CgiHandler::CgiHandler() : _writeClosed(false)
{}

CgiHandler::CgiHandler(const std::string& scriptPath, const std::string& interpreter, const HttpRequest& req,
	const std::string& clientIp, const std::string& serverName, unsigned int serverPort)
		: _scriptPath(scriptPath), _interpreter(interpreter), _pid(-1), _readFd(-1), _writeFd(-1), _writeClosed(false), _exitStatus(-1)
{
	_buildEnv(req, clientIp, serverName, serverPort);
}

CgiHandler::CgiHandler(const CgiHandler& other) : _scriptPath(other._scriptPath), _interpreter(other._interpreter),
      _env(other._env), _pid(other._pid), _readFd(other._readFd), _writeFd(other._writeFd), _writeClosed(other._writeClosed),
	  _output(other._output), _exitStatus(other._exitStatus)
{}

CgiHandler& CgiHandler::operator=(const CgiHandler& other)
{
	if (this != &other)
	{
		_scriptPath = other._scriptPath;
		_interpreter = other._interpreter;
		_env = other._env;
		_pid = other._pid;
		_readFd = other._readFd;
		_writeFd = other._writeFd;
		_output = other._output;
		_exitStatus = other._exitStatus;
		_writeClosed = other._writeClosed;
	}
	return *this;
}

CgiHandler::~CgiHandler()
{
	if (_readFd != -1)
		close(_readFd);
    if (_writeFd != -1)
		close(_writeFd);
    if (_pid != -1)
    {
        kill(_pid, SIGKILL);
        waitpid(_pid, NULL, 0);
    }
}

bool CgiHandler::start()
{
	int pipeIn[2];   // server to CGI
	int pipeOut[2];  // CGI to server

	if (pipe(pipeIn) == -1 || pipe(pipeOut) == -1)
		return false;

	_pid = fork();
	if (_pid == -1)
	{
		close(pipeIn[0]); close(pipeIn[1]);
		close(pipeOut[0]); close(pipeOut[1]);
		return false;
	}

	if (_pid == 0)
	{
		// Process children
		dup2(pipeIn[0], STDIN_FILENO);
		dup2(pipeOut[1], STDOUT_FILENO);
		close(pipeIn[0]); close(pipeIn[1]);
		close(pipeOut[0]); close(pipeOut[1]);

		std::string dir = _scriptPath.substr(0, _scriptPath.find_last_of('/'));
		if (!dir.empty()) chdir(dir.c_str());

		std::vector<char*> argv;
		argv.push_back(const_cast<char*>(_interpreter.c_str()));
		argv.push_back(const_cast<char*>(_scriptPath.c_str()));
		argv.push_back(NULL);

		std::vector<char*> envp;
		for (size_t i = 0; i < _env.size(); ++i)
			envp.push_back(const_cast<char*>(_env[i].c_str()));
		envp.push_back(NULL);

		execve(_interpreter.c_str(), argv.data(), envp.data());
		_exit(127); // failed
	}

	// Process father
	close(pipeIn[0]);
	close(pipeOut[1]);
	_writeFd = pipeIn[1];
	_readFd = pipeOut[0];

	// NONBLOCK
	fcntl(_writeFd, F_SETFL, O_NONBLOCK);
	fcntl(_readFd, F_SETFL, O_NONBLOCK);

	return true;
}

int CgiHandler::getReadFd() const
{
	return _readFd;
}

int CgiHandler::getWriteFd() const
{
	return _writeFd;
}

bool CgiHandler::isRunning() const
{
	return _pid != -1;
}

bool CgiHandler::isWriteClosed() const
{
	return _writeClosed;
}

bool CgiHandler::sendBody(const std::string& body)
{
	if (_writeClosed)
		return false;

	_pendingBody = body;
    _pendingOffset = 0;
	return flushPending();
}

bool CgiHandler::flushPending()
{
	while (_pendingOffset < _pendingBody.size())
	{
		ssize_t w = write(_writeFd, _pendingBody.data() + _pendingOffset, _pendingBody.size() - _pendingOffset);
		if (w < 0)
			return true;
		if (w == 0)
			return false;
		_pendingOffset += w;
	}
	closeWriteFd();
	return true;
}

void CgiHandler::closeWriteFd()
{
	if (!_writeClosed && _writeFd != -1)
	{
		close(_writeFd);
		_writeFd = -1;
		_writeClosed = true;
	}
}

ssize_t CgiHandler::readOutput()
{
	char buffer[4096];
	ssize_t n = read(_readFd, buffer, sizeof(buffer));

	if (n > 0)
		_output.append(buffer, n);
	else if (n == 0)
	{
		close(_readFd);
		_readFd = -1;
	}
	return n;
}

const std::string& CgiHandler::getOutput() const
{
	return _output;
}

bool CgiHandler::reapIfDone()
{
	if (_pid == -1) return false;

	int status = 0;
	pid_t r = waitpid(_pid, &status, WNOHANG);
	if (r == _pid)
	{
		if (WIFEXITED(status))
			_exitStatus = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			_exitStatus = 128 + WTERMSIG(status);
		else
			_exitStatus = -1;
		_pid = -1;
		return true;
	}
	return false;
}

int  CgiHandler::getExitStatus() const
{
	return _exitStatus;
}

HttpResponse CgiHandler::buildResponse() const
{
	return _parseCgiOutput();
}
