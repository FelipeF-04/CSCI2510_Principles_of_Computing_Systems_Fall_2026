#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#define bufferSize 200

int main(int argc, char *argv[]) {
    char buffer[bufferSize];
    ssize_t bytesRead;

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("Can't open file");
        return 1;
    }

    while (1) {
        bytesRead = read(fd, buffer, bufferSize);

        if (bytesRead == 0) {
            break;
        }

        if (bytesRead == -1) {
            perror("Error reading file");
            return 1;
        }

        write(STDOUT_FILENO, buffer, bytesRead);
    }

    close(fd);
    return 0;
}
