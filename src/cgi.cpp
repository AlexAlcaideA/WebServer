bool cgiexec(int type, char **av, char **env, int infd, int outfd, char *file)
{
pid_t pid = fork();

if (pid == 0)
{
    dup2(inputFd, STDIN_FILENO);
    dup2(outputFd, STDOUT_FILENO);

    char *argv[] = {
        (char *)"/usr/bin/python3",
        (char *)file,
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
}
}
