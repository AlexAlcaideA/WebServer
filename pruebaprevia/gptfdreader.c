#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static const char *file_type(mode_t mode)
{
    if (S_ISREG(mode))  return "regular file";
    if (S_ISDIR(mode))  return "directory";
    if (S_ISLNK(mode))  return "symlink";
    if (S_ISFIFO(mode)) return "FIFO";
    if (S_ISSOCK(mode)) return "socket";
    if (S_ISCHR(mode))  return "character device";
    if (S_ISBLK(mode))  return "block device";
    return "unknown";
}

static void print_flags(int fd)
{
    int flags = fcntl(fd, F_GETFL);

    if (flags == -1) {
        perror("fcntl(F_GETFL)");
        return;
    }

    printf("status flags: 0x%x\n", flags);
    printf("  access: ");

    switch (flags & O_ACCMODE) {
    case O_RDONLY: printf("read-only"); break;
    case O_WRONLY: printf("write-only"); break;
    case O_RDWR:   printf("read-write"); break;
    }

    printf("\n");
    printf("  O_APPEND:    %s\n", flags & O_APPEND ? "yes" : "no");
    printf("  O_NONBLOCK:  %s\n", flags & O_NONBLOCK ? "yes" : "no");
    printf("  O_ASYNC:     %s\n", flags & O_ASYNC ? "yes" : "no");
#ifdef O_DIRECT
    printf("  O_DIRECT:    %s\n", flags & O_DIRECT ? "yes" : "no");
#endif
}

static void print_fd_flags(int fd)
{
    int flags = fcntl(fd, F_GETFD);

    if (flags == -1) {
        perror("fcntl(F_GETFD)");
        return;
    }

    printf("FD flags: 0x%x\n", flags);
    printf("  FD_CLOEXEC:  %s\n",
           flags & FD_CLOEXEC ? "yes" : "no");
}

static void print_proc_link(int fd)
{
    char path[64];
    char target[PATH_MAX];

    snprintf(path, sizeof(path), "/proc/self/fd/%d", fd);

    ssize_t n = readlink(path, target, sizeof(target) - 1);
    if (n < 0) {
        perror("readlink(/proc/self/fd)");
        return;
    }

    target[n] = '\0';

    printf("/proc/self/fd: %s -> %s\n", path, target);
}

static void print_pipe_info(int fd)
{
#ifdef F_GETPIPE_SZ
    int size = fcntl(fd, F_GETPIPE_SZ);

    if (size >= 0)
        printf("pipe capacity: %d bytes\n", size);
#endif

    int bytes = 0;

    if (ioctl(fd, FIONREAD, &bytes) == 0)
        printf("bytes available to read: %d\n", bytes);
}

static void print_socket_info(int fd)
{
    int type;
    socklen_t len = sizeof(type);

    if (getsockopt(fd, SOL_SOCKET, SO_TYPE, &type, &len) == -1)
        return;

    printf("socket type: ");

    switch (type) {
    case SOCK_STREAM:    printf("SOCK_STREAM"); break;
    case SOCK_DGRAM:     printf("SOCK_DGRAM"); break;
    case SOCK_RAW:       printf("SOCK_RAW"); break;
    case SOCK_SEQPACKET: printf("SOCK_SEQPACKET"); break;
    default:             printf("%d", type); break;
    }

    printf("\n");

    int domain = 0;
    len = sizeof(domain);

    /*
     * SO_DOMAIN is Linux-specific.
     */
#ifdef SO_DOMAIN
    if (getsockopt(fd, SOL_SOCKET, SO_DOMAIN, &domain, &len) == 0)
        printf("socket domain: %d\n", domain);
#endif

    int sndbuf;
    len = sizeof(sndbuf);

    if (getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &sndbuf, &len) == 0)
        printf("send buffer: %d bytes\n", sndbuf);

    int rcvbuf;
    len = sizeof(rcvbuf);

    if (getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &rcvbuf, &len) == 0)
        printf("receive buffer: %d bytes\n", rcvbuf);

    struct sockaddr_storage addr;
    len = sizeof(addr);

    if (getsockname(fd, (struct sockaddr *)&addr, &len) == 0) {
        printf("local address: available (%u bytes)\n",
               (unsigned)len);
    }

    len = sizeof(addr);

    if (getpeername(fd, (struct sockaddr *)&addr, &len) == 0) {
        printf("peer address: available (%u bytes)\n",
               (unsigned)len);
    } else if (errno != ENOTCONN) {
        perror("getpeername");
    }
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "usage: %s FD\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *end;
    long value = strtol(argv[1], &end, 10);

    if (*end != '\0' || value < 0 || value > INT_MAX) {
        fprintf(stderr, "invalid FD: %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    int fd = (int)value;

    /*
     * Generic metadata
     */
    struct stat st;

    if (fstat(fd, &st) == -1) {
        perror("fstat");
        return EXIT_FAILURE;
    }

    printf("FD:              %d\n", fd);
    printf("type:            %s\n", file_type(st.st_mode));
    printf("mode:            %o\n", st.st_mode & 07777);
    printf("UID:             %u\n", st.st_uid);
    printf("GID:             %u\n", st.st_gid);
    printf("inode:           %lu\n", (unsigned long)st.st_ino);
    printf("device:          %lu\n", (unsigned long)st.st_dev);
    printf("size:            %lld bytes\n",
           (long long)st.st_size);
    printf("links:           %lu\n",
           (unsigned long)st.st_nlink);

    printf("\n");

    /*
     * FD-specific metadata
     */
    print_fd_flags(fd);

    printf("\n");

    /*
     * Open-file-description metadata
     */
    print_flags(fd);

    off_t offset = lseek(fd, 0, SEEK_CUR);

    if (offset != (off_t)-1)
        printf("current offset:  %lld\n", (long long)offset);
    else
        printf("current offset:  N/A (%s)\n", strerror(errno));

    printf("\n");

    /*
     * Kernel's description of what the FD refers to.
     */
    print_proc_link(fd);

    printf("\n");

    /*
     * Type-specific information
     */
    if (S_ISFIFO(st.st_mode)) {
        printf("[pipe/FIFO metadata]\n");
        print_pipe_info(fd);
    }

    if (S_ISSOCK(st.st_mode)) {
        printf("[socket metadata]\n");
        print_socket_info(fd);
    }

    return EXIT_SUCCESS;
}
