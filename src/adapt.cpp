#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <poll.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 4096

int main(void)
{
	int			server_fd;
	int			client_fd;
	int			ret;
	int			opt = 1;
	ssize_t		n;
	struct sockaddr_in	addr;
	struct pollfd		pfds[2];
	char			buffer[BUFFER_SIZE + 1];

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1)
	{
		perror("socket");
		return (1);
	}

	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR,
			&opt, sizeof(opt)) == -1)
	{
		perror("setsockopt");
		close(server_fd);
		return (1);
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(PORT);

	if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
	{
		perror("bind");
		close(server_fd);
		return (1);
	}

	if (listen(server_fd, SOMAXCONN) == -1)
	{
		perror("listen");
		close(server_fd);
		return (1);
	}

	printf("Listening on port %d...\n", PORT);

	memset(pfds, 0, sizeof(pfds));

	pfds[0].fd = server_fd;
	pfds[0].events = POLLIN;

	pfds[1].fd = -1;

	while (1)
	{
		ret = poll(pfds, 2, -1);
		if (ret == -1)
		{
			if (errno == EINTR)
				continue;

			perror("poll");
			break;
		}

		/*
		 * New connection.
		 */
		if (pfds[0].revents & POLLIN)
		{
			client_fd = accept(server_fd, NULL, NULL);
			if (client_fd == -1)
			{
				perror("accept");
				continue;
			}

			printf("Accepted client fd=%d\n", client_fd);

			pfds[1].fd = client_fd;
			pfds[1].events = POLLIN;
		}

		/*
		 * Existing client sent data.
		 */
		if (pfds[1].fd != -1 &&
			(pfds[1].revents & POLLIN))
		{
			n = recv(pfds[1].fd, buffer, BUFFER_SIZE, 0);

			if (n == -1)
			{
				perror("recv");
				close(pfds[1].fd);
				pfds[1].fd = -1;
				continue;
			}

			if (n == 0)
			{
				printf("Client disconnected\n");
				close(pfds[1].fd);
				pfds[1].fd = -1;
				continue;
			}

			buffer[n] = '\0';

			printf("----- REQUEST -----\n");
			printf("%s", buffer);
			printf("----- END REQUEST -----\n");

			/*
			 * Send response.
			 */
			if (send(pfds[1].fd, "OK\n", 3, 0) == -1)
			{
				perror("send");
			}

			close(pfds[1].fd);
			pfds[1].fd = -1;
		}
	}

	close(server_fd);
	return (0);
}
