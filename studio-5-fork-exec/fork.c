#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t child_PID = fork();
    char* child_argv[] = {"ls", "-l", NULL};
    char* child_prog = child_argv[0];

    if (child_PID < 0) {
        perror("fork failed");
        return 1;
    } 
    else if (child_PID == 0){
        //child
        sleep(3);
        printf("Child Process. Process ID: %d\n", getpid());

        execvp(child_prog, child_argv);
        // If execvp returns, it means an error occurred
        perror("execvp failed");
        return 1;
    } 
    else{ 
        //parent
        waitpid( child_PID, NULL, 0 );
        printf("Parent Process. Process ID: %d\n", getpid());
    }

    return 0;
}
