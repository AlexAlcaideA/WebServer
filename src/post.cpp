#include "defaultresponse.hpp"
#include "server.hpp"
#include "httprequest.hpp"
#include "includes.h"

int main()
{
	server Server(8081);
    while (true)
    {
        int client_fd = accept(server_fd, NULL, NULL);

        if (client_fd == -1)
        {
            perror("accept");
            continue;
        }

        // ==========================================
        // Leer request
        // ==========================================

        char buffer[4096];
        std::memset(buffer, 0, sizeof(buffer));

        int bytes = read(client_fd, buffer, sizeof(buffer));

        if (bytes <= 0)
        {
            close(client_fd);
            continue;
        }

        std::string request(buffer, bytes);

        std::cout << "----- REQUEST -----\n";
        std::cout << request;
        std::cout << "-------------------\n";

        // ==========================================
        // Comprobar método
        // ==========================================

        if (request.find("POST ") == 0)
        {
            // --------------------------------------
            // Buscar Content-Length
            // --------------------------------------

            size_t content_length_pos =
                request.find("Content-Length:");

            if (content_length_pos == std::string::npos)
            {
                std::string response = ;

                write(client_fd, response.c_str(), response.size());

                close(client_fd);
                continue;
            }

            // --------------------------------------
            // Obtener Content-Length
            // --------------------------------------

            size_t value_start = content_length_pos + 15;

            size_t value_end = request.find("\r\n", value_start);

            std::string length_string =request.substr(value_start,value_end - value_start);

            int content_length = std::atoi(length_string.c_str());

            std::cout << "Content-Length: "<< content_length << "\n";

            // --------------------------------------
            // Encontrar comienzo del body
            // --------------------------------------

            size_t body_start =
                request.find("\r\n\r\n");

            if (body_start == std::string::npos)
            {
                std::string response =
                    "HTTP/1.1 400 Bad Request\r\n"
                    "Content-Length: 0\r\n"
                    "Connection: close\r\n"
                    "\r\n";

                write(client_fd, response.c_str(), response.size());

                close(client_fd);
                continue;
            }

            body_start += 4;

            // --------------------------------------
            // Obtener body que ya hemos recibido
            // --------------------------------------

            std::string body =
                request.substr(body_start);

            // --------------------------------------
            // Si faltan bytes, seguir leyendo
            // --------------------------------------

            while ((int)body.size() < content_length)
            {
                char tmp[4096];

                int n = read(
                    client_fd,
                    tmp,
                    sizeof(tmp)
                );

                if (n <= 0)
                    break;

                body.append(tmp, n);
            }

            // Recortar por seguridad
            if ((int)body.size() > content_length)
                body.resize(content_length);

            std::cout << "BODY:\n";
            std::cout << body << "\n";

            // --------------------------------------
            // Guardar body
            // --------------------------------------

            std::ofstream file("upload.txt");

            if (!file)
            {
                std::string response =
                    "HTTP/1.1 500 Internal Server Error\r\n"
                    "Content-Length: 0\r\n"
                    "Connection: close\r\n"
                    "\r\n";

                write(client_fd, response.c_str(), response.size());

                close(client_fd);
                continue;
            }

            file << body;
            file.close();

            // --------------------------------------
            // Respuesta
            // --------------------------------------

            std::string response =
                "HTTP/1.1 201 Created\r\n"
                "Content-Length: 0\r\n"
                "Connection: close\r\n"
                "\r\n";

            write(client_fd, response.c_str(), response.size());
        }
        else
        {
            std::string response =
                "HTTP/1.1 405 Method Not Allowed\r\n"
                "Content-Length: 0\r\n"
                "Connection: close\r\n"
                "\r\n";

            write(client_fd, response.c_str(), response.size());
        }

        close(client_fd);
    }

    close(server_fd);
    return 0;
}
