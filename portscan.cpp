#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <vector>

struct ScanSocket {
    int fd;
    int puerto;
};

int main(int ac, char** av) 
{
	std::string ip;//ip en la que busca
	if (ac!=2){
    ip = "127.0.0.1";}
	else
	{
		ip = av[1];
	}

    std::vector<ScanSocket> sockets;
    std::vector<pollfd> pollfds;

    for (int puerto = 1; puerto <= 1024; puerto++) { //itera por puertos

        int sock = socket(AF_INET, SOCK_STREAM, 0);//crea el socket
        if (sock < 0)
            continue;

        // Socket no bloqueante
        fcntl(sock, F_SETFL, O_NONBLOCK);

        sockaddr_in addr{};
        addr.sin_family = AF_INET;//usa ipv4
        addr.sin_port = htons(puerto);//decide el puerto
        inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);

        int r = connect(sock, (sockaddr*)&addr, sizeof(addr));//intenta iniciar conexion tcp

        if (r == 0) {//hay conexion
            std::cout << puerto << " abierto\n";
            close(sock);
        }
        else if (errno == EINPROGRESS) {

            pollfd pfd{};
            pfd.fd = sock;
            pfd.events = POLLOUT;

            pollfds.push_back(pfd);
            sockets.push_back({sock, puerto});
        }
        else {
            close(sock);
        }
    }

    int timeout = 1000;

    int ready = poll(pollfds.data(), pollfds.size(), timeout);

    if (ready > 0) {

        for (size_t i = 0; i < pollfds.size(); i++) {

            if (pollfds[i].revents & POLLOUT) {

                int err;
                socklen_t len = sizeof(err);

                getsockopt(
                    pollfds[i].fd,
                    SOL_SOCKET,
                    SO_ERROR,
                    &err,
                    &len
                );

                if (err == 0)
                    std::cout << sockets[i].puerto << " abierto\n";

                close(pollfds[i].fd);
            }
        }
    }
    else {
        for (auto &p : pollfds)
            close(p.fd);
    }

    return 0;
}
