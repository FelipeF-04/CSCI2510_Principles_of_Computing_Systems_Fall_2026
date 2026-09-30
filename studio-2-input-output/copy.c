#include <unistd.h>

#define bufferSize 200

int main() {
    char buffer[bufferSize];
    ssize_t bytesRead;

    while (1) {
        bytesRead = read(STDIN_FILENO, buffer, bufferSize);
        if (bytesRead == 0) {
            break;
        }

        if (bytesRead == -1) {
            return 1;
        }
        write(STDOUT_FILENO, buffer, bytesRead);
    }
    return 0;
}

