#include "defaultresponse.hpp"
#include "server.hpp"
#include "httprequest.hpp"
#include "includes.h"


int main()
{
	server Server(8081);
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
