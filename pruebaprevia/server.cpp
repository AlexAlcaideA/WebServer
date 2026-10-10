#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#define VERSION 1.1
#define STATUSCODE 200 OK
#define CONTENT "Hello, World!";
#define PORT 8080
int	main(void)
{
	int tcpSocket4 = socket(AF_INET, SOCK_STREAM, 0);
	int tcpSocket6 = socket(AF_INET6, SOCK_STREAM, 0);
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (sockaddr*)&address, sizeof(address));
    listen(server_fd, 1);

    std::cout << "Listening on http://localhost:PORT\n";

    while (true) {
        int client = accept(server_fd, nullptr, nullptr);

        char buffer[1024];
        read(client, buffer, sizeof(buffer)); // Ignore request

        const char* response =
            "HTTP/VERSION STATUSCODE STATUSMSG\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 13\r\n"
            "\r\n"
			CONTENT

        write(client, response, strlen(response));
        close(client);
    }

    close(server_fd);
    return 0;
}
