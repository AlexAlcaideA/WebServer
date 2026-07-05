#include <iostream>
#include <string>
#include "../include/InhertedDirective.hpp"

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cerr << "Need a configuration file." << std::endl;
		return 1;
	}
	else 
	{
		std::string confRoot = argv[1];
	}
	(void)argv;
	return 0;
}