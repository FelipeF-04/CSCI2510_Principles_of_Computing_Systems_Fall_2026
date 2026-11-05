#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

#define MAX_LINE 256
#define MAX_ARGS 15
#define MAX_CMDS 64

static volatile sig_atomic_t interrupted = 0;

void handle_sigint(int sig) {
    (void)sig;
    interrupted = 1;
    write(STDOUT_FILENO, "\n", 1);
}

void prompt(void) {
    char cwd[1024];
    char *home = getenv("HOME");

    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        if (home && strncmp(cwd, home, strlen(home)) == 0 &&
            (cwd[strlen(home)] == '/' || cwd[strlen(home)] == '\0')) {
            char *relative = cwd + strlen(home);
            if (*relative == '/')
                relative++;
            printf("slush|%s> ", relative);
        } else {
            printf("slush|%s> ", cwd);
        }
    } else {
        printf("slush> ");
    }

    fflush(stdout);
}

int main(void) {
    char line[MAX_LINE];
    struct sigaction sa;

    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        return EXIT_FAILURE;
    }

    while (1) {
        interrupted = 0;
        prompt();

        if (fgets(line, sizeof(line), stdin) == NULL) {
            if (interrupted) {
                clearerr(stdin);
                continue;
            }
            printf("\n");
            break;
        }

        if (interrupted)
            continue;

        line[strcspn(line, "\n")] = '\0';

        /* Ignore empty or whitespace-only lines. */
        if (strspn(line, " \t") == strlen(line))
            continue;

        /* Handle cd separately because it must be on its own line. */
        if (strchr(line, '(') == NULL) {
            char temp[MAX_LINE];
            strcpy(temp, line);

            char *word = strtok(temp, " \t");
            if (word != NULL && strcmp(word, "cd") == 0) {
                char *dir = strtok(NULL, " \t");
                char *extra = strtok(NULL, " \t");

                if (extra != NULL) {
                    fprintf(stderr, "Invalid cd command\n");
                } else {
                    if (dir == NULL)
                        dir = getenv("HOME");

                    if (dir == NULL || chdir(dir) != 0)
                        perror("cd");
                }
                continue;
            }
        }

        /* Split the input into commands at each left parenthesis. */
        char *commands[MAX_CMDS];
        char *current = line;
        int count = 0;
        char *pos;
        int too_many_commands = 0;

        while ((pos = strchr(current, '(')) != NULL) {
            /* Leave room for the final command after the last parenthesis. */
            if (count >= MAX_CMDS - 1) {
                fprintf(stderr, "Too many commands\n");
                too_many_commands = 1;
                break;
            }

            *pos = '\0';
            commands[count++] = current;
            current = pos + 1;
        }

        if (too_many_commands)
            continue;

        commands[count++] = current;

        /* Separate each command into its arguments. */
        char *args[MAX_CMDS][MAX_ARGS + 2];
        int valid = 1;

        for (int i = 0; i < count; i++) {
            int n = 0;
            char *word = strtok(commands[i], " \t");

            while (word != NULL) {
                /* One program name plus up to 15 arguments. */
                if (n >= MAX_ARGS + 1) {
                    fprintf(stderr, "Too many arguments\n");
                    valid = 0;
                    break;
                }

                args[i][n++] = word;
                word = strtok(NULL, " \t");
            }

            args[i][n] = NULL;

            if (!valid)
                break;

            if (n == 0) {
                fprintf(stderr, "Invalid null command\n");
                valid = 0;
                break;
            }
        }

        if (!valid)
            continue;

        /* Execute commands from right to left, connecting them with pipes. */
        pid_t children[MAX_CMDS];
        int child_count = 0;
        int input_fd = -1;

        for (int i = count - 1; i >= 0; i--) {
            int pipefd[2] = {-1, -1};

            if (i > 0 && pipe(pipefd) == -1) {
                perror("pipe");
                break;
            }

            pid_t pid = fork();

            if (pid == -1) {
                perror("fork");
                if (pipefd[0] != -1) {
                    close(pipefd[0]);
                    close(pipefd[1]);
                }
                break;
            }

            if (pid == 0) {
                /* Commands should respond to Ctrl+C normally. */
                signal(SIGINT, SIG_DFL);

                if (input_fd != -1) {
                    dup2(input_fd, STDIN_FILENO);
                    close(input_fd);
                }

                if (i > 0) {
                    close(pipefd[0]);
                    dup2(pipefd[1], STDOUT_FILENO);
                    close(pipefd[1]);
                }

                execvp(args[i][0], args[i]);
                perror(args[i][0]);
                exit(EXIT_FAILURE);
            }

            children[child_count++] = pid;

            if (input_fd != -1)
                close(input_fd);

            if (i > 0) {
                close(pipefd[1]);
                input_fd = pipefd[0];
            }
        }

        if (input_fd != -1)
            close(input_fd);

        /* Wait for every child process to finish. */
        for (int i = 0; i < child_count; i++) {
            while (waitpid(children[i], NULL, 0) == -1) {
                if (errno != EINTR) {
                    perror("waitpid");
                    break;
                }
            }
        }
    }

    return 0;
}
