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

	const std::string rawRequest =
		"GET /favicon.ico HTTP/1.1\r\n"
		"Host: localhost:8081\r\n"
		"Connection: keep-alive\r\n"
		"sec-ch-ua-platform: \"Linux\"\r\n"
		"User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/139.0.0.0 Safari/537.36\r\n"
		"sec-ch-ua: \"Not;A=Brand\";v=\"99\", \"Google Chrome\";v=\"139\", \"Chromium\";v=\"139\"\r\n"
		"sec-ch-ua-mobile: ?0\r\n"
		"Accept: image/avif,image/webp,image/apng,image/svg+xml,image/*,*/*;q=0.8\r\n"
		"Sec-Fetch-Site: same-origin\r\n"
		"Sec-Fetch-Mode: no-cors\r\n"
		"Sec-Fetch-Dest: image\r\n"
		"Referer: http://localhost:8081/\r\n"
		"Accept-Encoding: gzip, deflate, br, zstd\r\n"
		"Accept-Language: en-US,en;q=0.9\r\n"
		"\r\n";  // línea vacía final que marca el fin de las cabeceras

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
		HttpRequest testRequest(rawRequest);
		std::cout << testRequest << std::endl;
	}
	catch(const std::exception& e)
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
