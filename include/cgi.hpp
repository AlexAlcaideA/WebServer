#ifndef CGI_HPP
# define CGI_HPP
class cgi
{
	private:
		std::map<std::string, std::string> env;
		pid_t		_pid;	// Process ID of the spawned CGI child
		int	_pipeIn[2];	// Pipe: Parent writes request body -> Child reads STDIN
		int	_pipeOut[2];	// Pipe: Child writes output STDOUT -> Parent reads
		time_t	_startTime;	// Timestamp when the child was spawned (for timeouts)

		std::string	_bodyToWrite;	// Holds remaining POST request body to send to CGI
		std::string	_outputBuffer;	// Accumulates raw output read from CGI
		bool		_isFinished;	// Set to true when CGI closes stdout or exits

	protected:
	public:

	~cgi(void);// Destructor default
	cgi(void);// Constructor default
	cgi();// Constructor parametrizado
	cgi(const cgi& otro);// Constructor copia
	cgi &operator= (const cgi& otro);// Sobrecarga operador asignacion

	bool execute(char** envp, const std::string& scriptPath, const std::string& execPath, const std::string& body);
	void handleWriteEvent(); // Writes chunk of _bodyToWrite to _stdinPipe[1]
	void handleReadEvent();  // Reads chunk from _stdoutPipe[0] into _outputRead

	// Non-blocking process & timeout management
	bool	checkTimeout(time_t timeoutSeconds);
	bool	isFinished();
	void	closePipes();
	void	setNonBlocking(int fd);

	int	getWriteFd() const;
	int	getReadFd() const;
	const std::string& getOutput() const;
	static void	validateCgiPaths(const std::string& scriptPath, const std::string& execPath);
	static char**	envArrayFromMap(const std::map<std::string, std::string>& env);
	static bool	canHandleCgi(const t_uri& uri, const LocationConfig& conf);
	static char**	buildCgiEnv(const Request& req, const ServerConfig& conf, const std::string& ip);
	static void	freeCgiEnv(char** envp);
};

#endif
