#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main() {
    char in_string[256];
    int max_args = 15;
    int max_argv_size = max_args + 2;
    char* cmd;
    char* my_argv[max_argv_size];

    printf("Input: ");
    fgets(in_string, sizeof(in_string), stdin);

    if (strchr(in_string, '\n') != NULL) {
        *strchr(in_string, '\n') = '\0';
    }

    //cmd = first token
    cmd = strtok(in_string, " ");

    // no input
    if (cmd == NULL) {
        fprintf(stderr, "No command entered.\n");
        return 1;
    }

    my_argv[0] = cmd;
    int i = 1;
    char* res = strtok(NULL, " ");

    while (res != NULL && i < max_argv_size - 1) {
        my_argv[i] = res;
        i++;
        res = strtok(NULL, " ");
    }
    my_argv[i] = NULL;

    execvp(cmd, my_argv);
    perror("execvp failed");
    
    return 1;
}