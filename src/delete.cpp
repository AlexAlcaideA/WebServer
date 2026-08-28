#include "defaultresponse.hpp"
#include "server.hpp"
#include "httprequest.hpp"
#include "includes.h"

bool sendResponse(int client_fd, const std::string& response)
{
    return write(client_fd, response.c_str(), response.size()) >= 0;
}

int main()
{
	server Server(8081);
    while (true)
    {
        char buffer[BUFF_SIZE];
        std::memset(buffer, 0, sizeof(buffer));

        int bytes = read(client_fd, buffer, sizeof(buffer) - 1);

        if (bytes <= 0)
        {
            close(client_fd);
            continue;
        }

        std::string received(buffer);

        std::cout << "----- REQUEST -----\n";
        std::cout << received;
        std::cout << "-------------------\n";
		
		httpreq request(received);

		DefaultResponse answer;
		std::string filename = .getPath();
        if (request.getMethod() == "DELETE")
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

    }
    return 0;
}
