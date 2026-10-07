#pragma once
#include "includes.hpp"
#include "CgiHandler.hpp"

class CgiManager
{
	private:
		std::vector<CgiHandler*> _active;

	public:
		CgiManager();
		~CgiManager();

		CgiHandler* startCgi(const std::string& scriptPath, const std::string& interpreter, const HttpRequest& req,
						const std::string& clientIp, const std::string& serverName, unsigned int serverPort);

		CgiHandler* findByReadFd(int fd);
		CgiHandler* findByWriteFd(int fd);

		void remove(CgiHandler* cgi);
		void reapFinished();
};