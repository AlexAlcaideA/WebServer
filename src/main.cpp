#include <exception>
#include <iostream>
#include <string>
#include "../include/Configuration.hpp"

int main(int argc, char **argv)
{
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

	try
	{
		Configuration conf(confRoot);
		std::cout << conf;
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() <<  std::endl;
		return 1;
	}

	return 0;
}
