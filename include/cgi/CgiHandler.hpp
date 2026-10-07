#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include "includes.hpp"
#include "httpMessage/HttpRequest.hpp"
#include "httpMessage/HttpResponse.hpp"

class CgiHandler
{
	private:
		std::string					_scriptPath;
		std::string					_interpreter;
		std::vector<std::string>	_env;
		pid_t						_pid;
		int							_readFd;
		int							_writeFd;
		bool						_writeClosed;
		std::string					_output;
		int							_exitStatus;
		std::string					_pendingBody;
		size_t						_pendingOffset;

		bool _buildEnv(const HttpRequest& req, const std::string& clientIp, const std::string& serverName, unsigned int serverPort);
		HttpResponse _parseCgiOutput() const;
		static std::string _dechunk(const std::string& body);

	public:
		CgiHandler();
		CgiHandler(const std::string& scriptPath, const std::string& interpreter, const HttpRequest& req,
				const std::string& clientIp, const std::string& serverName, unsigned int serverPort);
		CgiHandler(const CgiHandler& other);
		CgiHandler& operator=(const CgiHandler& other);
		~CgiHandler();

		bool start();

		int getReadFd() const;
		int getWriteFd() const;

		bool isRunning() const;
		bool isWriteClosed() const;

		bool sendBody(const std::string& body);
		bool flushPending();
		void closeWriteFd();

		ssize_t readOutput();
		const std::string& getOutput() const;

		bool reapIfDone();
		int  getExitStatus() const;

		HttpResponse buildResponse() const;
};

#endif