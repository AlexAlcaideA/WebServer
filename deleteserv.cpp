#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

bool sendResponse(int client_fd, const std::string& response)
{
    return write(client_fd, response.c_str(), response.size()) >= 0;
}

int main()
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        perror("socket");
        return 1;
    }

    sockaddr_in address;
    std::memset(&address, 0, sizeof(address));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    if (bind(server_fd, (sockaddr*)&address, sizeof(address)) == -1)
    {
        perror("bind");
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, 10) == -1)
    {
        perror("listen");
        close(server_fd);
        return 1;
    }

    std::cout << "Server listening on port 8080\n";

    while (true)
    {
        int client_fd = accept(server_fd, NULL, NULL);
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

        // --------------------------------
        // Obtener método
        // --------------------------------

        size_t first_space = request.find(' ');

        if (first_space == std::string::npos)
        {
            close(client_fd);
            continue;
        }

        std::string method = request.substr(0, first_space);

        // --------------------------------
        // Obtener path
        // --------------------------------

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
        if (path == "/")
            path = "/index.html";

        std::string filename = "." + path;


        // ==============================================
        // DELETE
        // ==============================================

        if (method == "DELETE")
        {
            if (remove(filename.c_str()) == 0)
            {
                std::string response =
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Length: 16\r\n"
                    "Content-Type: text/plain\r\n"
                    "Connection: close\r\n"
                    "\r\n"
                    "File deleted OK\n";

                sendResponse(client_fd, response);
            }
            else
            {
                std::string response =
                    "HTTP/1.1 404 Not Found\r\n"
                    "Content-Length: 13\r\n"
                    "Content-Type: text/plain\r\n"
                    "Connection: close\r\n"
                    "\r\n"
                    "404 Not Found";

                sendResponse(client_fd, response);
            }
        }

        else
        {
            std::string response =
                "HTTP/1.1 405 Method Not Allowed\r\n"
                "Content-Length: 18\r\n"
                "Content-Type: text/plain\r\n"
                "Connection: close\r\n"
                "\r\n"
                "Method Not Allowed";

            sendResponse(client_fd, response);
        }

        close(client_fd);
    }

    close(server_fd);
    return 0;
}
