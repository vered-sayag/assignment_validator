//vered sayag - 318211737
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <wait.h>
#include <fcntl.h>

#define ERROR "Error in system call"
#define WRITE_ERROR write(2, ERROR, sizeof(ERROR));exit(-1);

/**
 * Goes through the files and checks if they are the same
 *
 * @param file1
 * @param file2
 * @return  1 -if they Identical an 0 otherwise
 */
int Identical(char *file1, char *file2) {
    int fd1, fd2;
    char buffer1, buffer2;


        fd1 = open(file1, O_RDONLY);
        if (fd1 == -1) {
            WRITE_ERROR
        }


        fd2 = open(file2, O_RDONLY);
        if (fd2 == -1) {
            close(fd1);
            WRITE_ERROR
        }


    ssize_t read1;
    ssize_t read2;

    while (1) {

        read1 = read(fd1, &buffer1, 1);
        if (read1 == -1) {
            WRITE_ERROR
        }


        read2 = read(fd2, &buffer2, 1);
        if (read2 == -1) {
            WRITE_ERROR
        }


        if (read1 == 0 && read2 == 0) {
            close(fd1);
            close(fd2);
            return 1;

        } else if (read1 == 0 || read2 == 0) {
            close(fd1);
            close(fd2);
            return 0;
        }

        if (buffer1 == buffer2) {
            continue;
        } else {
            close(fd1);
            close(fd2);
            return 0;
        }
    }


}
/**
 * Goes through the files and checks if they are the similar
 *
 * @param file1
 * @param file2
 * @return  1 -if they Similar an 0 otherwise
 */
int Similar(char *file1, char *file2) {
    int fd1, fd2;
    char buffer1, buffer2;


        fd1 = open(file1, O_RDONLY);
        if (fd1 == -1) {
            WRITE_ERROR
        }

        fd2 = open(file2, O_RDONLY);
        if (fd2 == -1) {
            close(fd1);
            WRITE_ERROR
        }


    ssize_t read1;
    ssize_t read2;

    while (1) {

        while (read1 = read(fd1, &buffer1, 1) != 0) {
            if (read1 == -1) {
                WRITE_ERROR
            }
            if (buffer1 != '\n' && buffer1 != ' ')
                break;
        }

        while (read2 = read(fd2, &buffer2, 1) != 0) {
            if (read2 == -1) {
                WRITE_ERROR
            }
            if (buffer2 != '\n' && buffer2 != ' ')
                break;
        }

        if (read1 == 0 && read2 == 0) {
            close(fd1);
            close(fd2);
            return 1;

        } else if (read1 == 0 || read2 == 0) {
            close(fd1);
            close(fd2);
            return 0;
        }


        if ('a' <= buffer1 && 'z' >= buffer1) {
            buffer1 = buffer1 - 'a' + 'A';
        }
        if ('a' <= buffer2 && 'z' >= buffer2) {
            buffer2 = buffer2 - 'a' + 'A';
        }

        if (buffer1 == buffer2) {
            continue;
        } else {
            close(fd1);
            close(fd2);
            return 0;
        }
    }


}

int main(int argc, char *argv[]) {

    if (argc != 3) {
        exit(-1);
    }

    if (Identical(argv[1], argv[2])) {
        return 1;

    } else if (Similar(argv[1], argv[2])) {

        return 3;
    }
    return 2;

}