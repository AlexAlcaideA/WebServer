
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "defaultresponse.hpp"
#include "server.hpp"

bool sendResponse(int client_fd, const std::string& response)
{
    return write(client_fd, response.c_str(), response.size()) >= 0;
}

int main()
{
	server Server(8080);
    while (true)
    {
        int client_fd = accept(Server.server_fd, NULL, NULL);
        if (client_fd == -1)
        {
            perror("accept");
            continue;
        }

        char buffer[4096];
        std::memset(buffer, 0, sizeof(buffer));

        int bytes = read(client_fd, buffer, sizeof(buffer) - 1);

        if (bytes <= 0)
        {
            close(client_fd);
            continue;
        }

        std::string request(buffer);

        std::cout << "----- REQUEST -----\n";
        std::cout << request;
        std::cout << "-------------------\n";

        // Obtener método

        size_t first_space = request.find(' ');

        if (first_space == std::string::npos)//ultimo espacio posible
        {
            close(client_fd);
            continue;
        }

        std::string method = request.substr(0, first_space);

        // Obtener path

        size_t second_space = request.find(' ', first_space + 1);

        if (second_space == std::string::npos)
        {
            close(client_fd);
            continue;
        }

        std::string path = request.substr(
            first_space + 1,
            second_space - first_space - 1
        );

        std::cout << "Method: " << method << "\n";
        std::cout << "Path: " << path << "\n";

        // Evitamos acceder a "/" directamente
        if (path == "/" || path == "..")
            path = "/index.html";

        std::string filename = path;
	DefaultResponse answer;
        if (method == "DELETE")
        {
            if (remove(filename.c_str()) == 0)
            {
                sendResponse(client_fd, answer.getString(NO_CONTENT));
            }
            else
            {
                sendResponse(client_fd, answer.getString(NOT_FOUND));
            }
        }
        else
        {
            sendResponse(client_fd, answer.getString(METHOD_NOT_ALLOWED));
        }

        close(client_fd);
    }
    return 0;
}
