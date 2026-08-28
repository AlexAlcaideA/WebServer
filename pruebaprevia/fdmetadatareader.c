#include <stdio.h>
#include <fcntl.h>
#include <pipe.h>

int	main(int argc, char **argv, char **envp)
{
	if (argc >= 2)
	{
		int fd = atoi(argv[1]);
		printf("file descriptor: %i", fd);
		printf("flags: %i", fcntl(fd, F_GETFD));
		printf("nonblocking flags: %i", fcntl(fd, F_GETFL));
		printf("pipe flags: %i", fcntl(fd, F_GETPIPE_SZ));
	//	getsockname(fd, ...);// local address
	//	getpeername(fd, ...);// remote address
	//	getsockopt(fd, ...);// socket options/state
	}
	return (0);
}

