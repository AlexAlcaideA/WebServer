#include "cgi/CgiManager.hpp"

CgiManager::CgiManager()
{}

CgiManager::~CgiManager()
{
	for (size_t i = 0; i < _active.size(); i++)
	{
		if (_active[i])
		{
			delete _active[i];
			_active[i] = NULL;
		}
	}
	_active.clear();
}

CgiHandler* CgiManager::startCgi(const std::string& scriptPath, const std::string& interpreter, const HttpRequest& req,
				const std::string& clientIp, const std::string& serverName, unsigned int serverPort, const std::string& finalBody)
{
	CgiHandler* cgi = new CgiHandler(scriptPath, interpreter, req, clientIp, serverName, serverPort, finalBody);

	if (!cgi->start())
	{
		delete cgi;
		return NULL;
	}

	_active.push_back(cgi);
	return cgi;
}

CgiHandler* CgiManager::findByReadFd(int fd)
{
	for (size_t i = 0; i < _active.size(); ++i)
	{
		if (_active[i]->getReadFd() == fd)
			return _active[i];
	}
	return NULL;
}

CgiHandler* CgiManager::findByWriteFd(int fd)
{
	for (size_t i = 0; i < _active.size(); ++i)
	{
		if (_active[i]->getWriteFd() == fd)
			return _active[i];
	}
	return NULL;
}

void CgiManager::remove(CgiHandler* cgi)
{
	for (std::vector<CgiHandler*>::iterator it = _active.begin(); it != _active.end(); ++it)
	{
		if (*it == cgi)
        {
            delete *it;
            _active.erase(it);
            return;
        }
	}
}

void CgiManager::reapFinished()
{
	for (size_t i = 0; i < _active.size(); ++i)
		_active[i]->reapIfDone();
}

void CgiManager::checkTimeouts()
{
	for (size_t i = 0; i < _active.size(); ++i)
	{
		if (_active[i]->getTicks() > CGI_TIMEOUT_TICKS)
			_active[i]->killProcess();
	}
}

void CgiManager::tickAll()
{
	for (size_t i = 0; i < _active.size(); ++i)
		_active[i]->incrementTicks();
}

