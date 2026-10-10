#include "includes.hpp"

int main()
{
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

	return (0);
}
