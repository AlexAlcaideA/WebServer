#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

bool cgiexec(int type, char **av, char **env, int infd, int outfd, char *file)
{
pid_t pid = fork();

if (pid == 0)
{
    dup2(inputFd, STDIN_FILENO);
    dup2(outputFd, STDOUT_FILENO);

    char *argv[] = {
        (char *)"/usr/bin/python3",
        (char *)"script.py",
        NULL
    };

    char *envp[] = {
        (char *)"REQUEST_METHOD=GET",
        (char *)"QUERY_STRING=test=42",
        NULL
    };

    execve("/usr/bin/python3", argv, envp);

    // Only reached if execve fails
    perror("execve");
    exit(1);
}
}
int main()
{
    // 1. Crear socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        perror("socket");
        return 1;
    }

    // 2. Configurar dirección
    sockaddr_in address;
    std::memset(&address, 0, sizeof(address));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    // 3. Bind
    if (bind(server_fd,
             (sockaddr *)&address,
             sizeof(address)) == -1)
    {
        perror("bind");
        close(server_fd);
        return 1;
    }

    // 4. Listen
    if (listen(server_fd, 10) == -1)
    {
        perror("listen");
        close(server_fd);
        return 1;
    }

    std::cout << "Server listening on http://localhost:8080\n";

    while (true)
    {
        // 5. Aceptar cliente
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd == -1)
        {
            perror("accept");
            continue;
        }

        // 6. Leer request HTTP
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
		std::string path;

        // 7. Comprobar que sea GET /
		if (request.find("GET ") == 0)
		{
		    size_t start = 4;
		    size_t end = request.find(' ', start);

		    path = request.substr(start, end - start);

		    if (path == "/")
		        path = "/index.html";

		    std::string filename = "./docs" + path;
			// 8. Abrir archivo
		    std::ifstream file(filename.c_str());

            if (!file)
            {
                std::string response =
                    "HTTP/1.1 404 Not Found\r\n"
                    "Content-Length: 13\r\n"
                    "Content-Type: text/plain\r\n"
                    "Connection: close\r\n"
                    "\r\n"
                    "404 Not Found";

                write(client_fd, response.c_str(), response.size());
            }
            else
            {
                // 9. Leer archivo entero
                std::stringstream content;
                content << file.rdbuf();

                std::string body = content.str();

                // 10. Crear respuesta HTTP
                std::stringstream response;

                response << "HTTP/1.1 200 OK\r\n";
                response << "Content-Type: text/html\r\n";
                response << "Content-Length: " << body.size() << "\r\n";
                response << "Connection: close\r\n";
                response << "\r\n";
                response << body;

                std::string response_str = response.str();

                // 11. Enviar respuesta
                write(client_fd,
                      response_str.c_str(),
                      response_str.size());
            }
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

            write(client_fd, response.c_str(), response.size());
        }

        // 12. Cerrar conexión
        close(client_fd);
    }

    close(server_fd);
    return 0;
}
