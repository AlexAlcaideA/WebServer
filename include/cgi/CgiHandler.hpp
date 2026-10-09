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
		int							_clientFd;
		bool						_hasExited;
		size_t						_ticks;

		bool _buildEnv(const HttpRequest& req, const std::string& clientIp, const std::string& serverName, unsigned int serverPort);
		HttpResponse _parseCgiOutput() const;

	public:
		CgiHandler();
		CgiHandler(const std::string& scriptPath, const std::string& interpreter, const HttpRequest& req,
				const std::string& clientIp, const std::string& serverName, unsigned int serverPort, const std::string& finalBody);
		CgiHandler(const CgiHandler& other);
		CgiHandler& operator=(const CgiHandler& other);
		~CgiHandler();

		bool start();

		int  getClientFd() const;
		void setClientFd(int fd);

		int getReadFd() const;
		int getWriteFd() const;

		int getPid() const;

		const std::string& getPendingBody() const;
		void setPendingBody(const std::string& body);

		bool isRunning() const;
		bool isWriteClosed() const;

		bool sendBody(const std::string& body);
		bool flushPending();
		void closeWriteFd();

		void setContentLength(size_t length);

		ssize_t readOutput();
		const std::string& getOutput() const;

		bool reapIfDone();
		int  getExitStatus() const;
		bool getHasExited() const;

		HttpResponse buildResponse() const;

		void incrementTicks();
		size_t getTicks() const;
		void killProcess();
};

#endif