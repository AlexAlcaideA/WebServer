#include <iostream>
#include <string>
#include "../include/Configuration.hpp"
#include "includes.hpp"

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cerr << "Need a configuration file." << std::endl;
		return 1;
	}
	std::string confRoot = argv[1];
	Configuration conf(confRoot);
	std::cout << conf;
	try
	{
		server srv(8080);
		srv.run();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return (1);
	}

	return 0;
}
