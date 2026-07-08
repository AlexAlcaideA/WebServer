#include <iostream>
#include <string>
#include "../include/Configuration.hpp"

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cerr << "Need a configuration file." << std::endl;
		return 1;
	}
	std::string confRoot = argv[1];
	Configuration conf(confRoot);
	conf << std::cout;
	return 0;
}