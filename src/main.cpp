#include "includes.hpp"
#include "configuration/Configuration.hpp"
#include "server.hpp"

volatile sig_atomic_t g_running = 1;

void handleSignal(int signum)
{
    (void)signum;
    g_running = 0;
}

int main(int argc, char **argv)
{
	signal(SIGINT, handleSignal);
    signal(SIGTERM, handleSignal);

	std::string confRoot;
	if (argc < 2)
	{
		std::cout << "Using default config file." << std::endl;
		confRoot = "config/default-config.conf";
	}
	else if (argc == 2)
		confRoot = argv[1];
	else
	{
		std::cerr << "Too many arguments." << std::endl;
		return 1;
	}

	Configuration conf;
	try
	{
		Configuration tmp(confRoot);
		conf = tmp;
		std::cout << conf << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() <<  std::endl;
		return 1;
	}

	try
	{
		server srv(conf);
		srv.run();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return (1);
	}

	return 0;
}
