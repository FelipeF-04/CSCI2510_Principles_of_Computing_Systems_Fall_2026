
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int fd[2];

    // Create the pipe
    if (pipe(fd) == -1) {
        perror("pipe failed");
        return 1;
    }

    // Child 1
    pid_t pid1 = fork();
    if (pid1 == -1) {
        perror("Error forking child 1");
        return 1;
    }

    if (pid1 == 0) {
        // Child 1: redirect stdout to the pipe
        dup2(fd[1], STDOUT_FILENO);

        close(fd[0]);
        close(fd[1]);

        char* cmd = "./print";
        char* myargv[] = {"print", NULL};

        execvp(cmd, myargv);

        perror("Error exec'ing print");
        return 1;
    }

    // Child 2
    pid_t pid2 = fork();
    if (pid2 == -1) {
        perror("Error forking child 2");
        return 1;
    }

    if (pid2 == 0) {
        // Child 2: redirect stdin to the pipe
        dup2(fd[0], STDIN_FILENO);

        close(fd[0]);
        close(fd[1]);

        char* cmd = "./string_loop";
        char* myargv[] = {"string_loop", NULL};

        execvp(cmd, myargv);

        perror("Error exec'ing string_loop");
        return 1;
    }
    
    // Parent does not read or write to the pipe
    close(fd[0]);
    close(fd[1]);

    // Parent waits for both children
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    return 0;
}
