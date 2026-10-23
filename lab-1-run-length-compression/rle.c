//Runlength compression in C
// Usage: ./rle <input file> <output file> <runlength> <mode>
//        Where mode=0 is compress and mode=1 is decompress

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char *argv[]){
    // check the command is complete
    if (argc != 5){
        perror("Usage: ./rle <input file> <output file> <compression length> <mode>");
        exit(-1);
    }
    
    int compr_len = atoi(argv[3]);
    int mode = atoi(argv[4]);

    // check mode
    if (mode != 0 && mode != 1){
        perror("Error. There are only 2 modes: 0 to compress, 1 to decompress");
        exit(-1);
    }

    // check compression length:
    if (compr_len < 1){
        perror("Error. The compression lenth must be greater or equal to 1");
        exit(-1);
    }

    int input_file = open(argv[1], O_RDONLY);
    if (input_file < 0){
        perror("Error opening input file");
        exit(-1);
    }

    int out_file = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (out_file < 0){
        perror("Error opening output file");
        exit(-1);
    }

    // compress mode:
    if (mode == 0){
        unsigned char *current = malloc(compr_len);
        unsigned char *next = malloc(compr_len);
        if (!current || !next){
            perror("Error allocating memory");
            exit(-1);
        }

        // first group
        ssize_t b_read = read(input_file, current, compr_len);
        if (b_read < 0){
            perror("Error finding first pattern");
            exit(-1);
        }
        if (b_read == 0){
            free(current);
            free(next);

            if (close(input_file) < 0){
                perror("Error closing input file");
                exit(-1);
            }

            if (close(out_file) < 0){
                perror("Error closing output file");
                exit(-1);
            }

            return 0;
        }

        ssize_t current_len = b_read;
        unsigned char counter = 1;

        while (1) {
            b_read = read(input_file, next, compr_len);

            if (b_read < 0){
                perror("Error reading input file");
                exit(-1);
            }

            // end of file
            if (b_read == 0){
                if (write(out_file, &counter, 1) != 1 || write(out_file, current, current_len) != current_len){
                    perror("Error writing to output file");
                    exit(-1);
                }
                break;
            }

            // same pattern
            if (b_read == current_len &&
                memcmp(current, next, current_len) == 0){

                counter++;

                // maximum run length
                if (counter == 255){
                    if (write(out_file, &counter, 1) != 1 || write(out_file, current, current_len) != current_len){
                        perror("Error writing to output file");
                        exit(-1);
                    }

                    counter = 0;
                }

            } else {
                // new pattern, write the old one
                if (write(out_file, &counter, 1) != 1 || write(out_file, current, current_len) != current_len){
                    perror("Error writing to output file");
                    exit(-1);
                }

                // switch current and next
                unsigned char *temp = current;
                current = next;
                next = temp;

                current_len = b_read;
                counter = 1;
            }
        }

        free(current);
        free(next);
                
        if (close(input_file) < 0) {
            perror("Error closing input file");
            exit(-1);
        }
        if (close(out_file) < 0) {
            perror("Error closing output file");
            exit(-1);
        }

    // decompress mode:
    } else if (mode == 1){
        unsigned char count_byte;
        unsigned char *pattern = malloc(compr_len);
        if (!pattern) {
            perror("Error allocating memory");
            exit(-1);
        }

        while (1){
            ssize_t b_read = read(input_file, &count_byte, 1);
            if (b_read < 0){
                perror("Error reading input file");
                exit(-1);
            }
            if (b_read == 0){
                break; // done
            }

            //read pattern
            ssize_t pattern_read = read(input_file, pattern, compr_len);
            if (pattern_read < 0){
                perror("Error reading input file");
                exit(-1);
            }
            if (pattern_read == 0){
                perror("Error reading file during decompression");
                exit(-1);
            }

            //write count_byte many times
            for (unsigned char i = 0; i < count_byte; i++){
                if (write(out_file, pattern, pattern_read) != pattern_read){
                    perror("Error writing to output file");
                    exit(-1);
                }
            }
        }
        free(pattern);

        if (close(input_file) < 0) {
            perror("Error closing input file");
            exit(-1);
        }
        if (close(out_file) < 0) {
            perror("Error closing output file");
            exit(-1);
        }
    }
}