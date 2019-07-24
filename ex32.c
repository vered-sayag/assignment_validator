//vered sayag - 318211737
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <wait.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>

#define OUTPUT "output.txt"
#define MAX_LINE 151
#define ERROR "Error in system call"
#define WRITE_ERROR write(2, ERROR, sizeof(ERROR));exit(-1);
#define TIMEOUT  if (write(fdResults, ",40,TIMEOUT\n", 12) == -1) {WRITE_ERROR}
#define GREAT_JOB  if (write(fdResults, ",100,GREAT_JOB\n", 15) == -1) {WRITE_ERROR}
#define BAD_OUTPUT if (write(fdResults, ",60,BAD_OUTPUT\n", 15) == -1) {WRITE_ERROR}
#define SIMILAR_OUTPUT if (write(fdResults, ",80,SIMILAR_OUTPUT\n", 19) == -1) {WRITE_ERROR}
#define COMPILATION_ERROR if (write(fdResults, ",20,COMPILATION_ERROR\n", 22) == -1) {WRITE_ERROR}
#define NO_C_FILE if (write(fdResults, ",0,NO_C_FILE\n", 13) == -1) {WRITE_ERROR}

int Compile(char *fileCName);

int Run(char *inputsFile, int fdResults);

void Comper(char *nameOutputRunFile, int fdResults, char *workDir);

void FindC(char *nameCfile, char lines[3][MAX_LINE], int fdResults, char *workDir);

int SearchIn(char *name, char lines[3][MAX_LINE], int fdResults, char *workDir);

void Search(char *name, char lines[3][MAX_LINE], int fdResults, char *workDir);

void GetInput(char *file, char lins[3][MAX_LINE]);

int main(int argc, char *argv[]) {

    if (argc != 2) {
        return (-1);
    }

    char lins[3][MAX_LINE] = {0};
    GetInput(argv[1], lins);

    char workDir[MAX_LINE];
    getcwd(workDir, sizeof(workDir));

    int fdResults = open("results.csv", O_CREAT | O_TRUNC | O_WRONLY, 0644);
    if (fdResults == -1) {
        WRITE_ERROR
    }

    Search(lins[0], lins, fdResults, workDir);
    close(fdResults);
    return 0;
}

/***
 * compile the c file
 * @param fileCName
 * @return 0 - if success and 1- otherwise
 */
int Compile(char *fileCName) {

    char *params[3];
    params[0] = "gcc";
    params[1] = fileCName;
    params[2] = NULL;

    int status;
    pid_t pid;

    if ((pid = fork()) == -1) {
        WRITE_ERROR
    } else if (pid == 0) {

        execvp(params[0], params);
        WRITE_ERROR

    }
    waitpid(pid, &status, WUNTRACED);
    if (WEXITSTATUS(status) == 1) {
        return 0;
    }
    return 1;

}

/***
 * run the a.out file - print time out to the results file if time out Happens
 * @param inputsFile - a file with the args for the run
 * @param fdResults - fd of the results file
 * @return 0 - if success and 1- otherwise
 */
int Run(char *inputsFile, int fdResults) {

    char *params[2];
    params[0] = "./a.out";
    params[1] = NULL;

    int status, outfd, inputfd;
    pid_t pid;

    if ((outfd = open(OUTPUT, O_CREAT | O_TRUNC | O_WRONLY, 0644)) < 0) {
        WRITE_ERROR
    }
    if ((inputfd = open(inputsFile, O_RDONLY)) < 0) {
        WRITE_ERROR
    }


    if ((pid = fork()) == -1) {
        WRITE_ERROR
    } else if (pid == 0) {
        dup2(inputfd, 0);
        dup2(outfd, 1);
        execvp(params[0], params);
        WRITE_ERROR
    }

    sleep(5);
    if (!waitpid(pid, &status, WNOHANG)) {
        kill(pid, 0);
        close(outfd);
        close(inputfd);
        TIMEOUT
        return 0;
    }

    close(outfd);
    close(inputfd);
    return 1;

}

/***
 *Comper the output file of the run with the correct output
 * @param nameOutputRunFile -output file of the run
 * @param fdResults - fd of the results file
 * @param workDir - the path of the run of this program
 */
void Comper(char *nameOutputRunFile, int fdResults, char *workDir) {

    int status;
    pid_t pid;
    char *params[4];
    char tempworkDir[MAX_LINE];
    char outputPath[MAX_LINE];

    strcpy(tempworkDir, workDir);
    strcat(tempworkDir, "/comp.out");

    getcwd(outputPath, sizeof(outputPath));
    strcat(outputPath, "/");
    strcat(outputPath, OUTPUT);

    params[0] = tempworkDir;
    params[1] = nameOutputRunFile;
    params[2] = outputPath;
    params[3] = NULL;


    if ((pid = fork()) == -1) {
        WRITE_ERROR
    } else if (pid == 0) {
        execvp(params[0], params);
        WRITE_ERROR
    }
    waitpid(pid, &status, WUNTRACED);
    if (WEXITSTATUS(status) == 1) {
        GREAT_JOB
    }
    if (WEXITSTATUS(status) == 2) {
        BAD_OUTPUT
    }
    if (WEXITSTATUS(status) == 3) {
        SIMILAR_OUTPUT
    }

}


/**
 * compile run and comper whan c file found
 * @param nameCfile - c file
 * @param lines - the arguments -line[0] -the main folder - lines[1]- input file  -lines[2]- output file
 * @param fdResults - fd of the results file
 * @param workDir - the path of the run of this program
 */
void FindC(char *nameCfile, char lines[3][MAX_LINE], int fdResults, char *workDir) {

    if (Compile(nameCfile)) {
        if (Run(lines[1], fdResults)) {
            Comper(lines[2], fdResults, workDir);
        }
        if (unlink(OUTPUT) == -1) {
            WRITE_ERROR
        }
        if (unlink("a.out") == -1) {
            WRITE_ERROR
        }
    } else {
        COMPILATION_ERROR
    }

}

/**
 * Searching inside folder and subfolders c file
 * @param name - name folder
 * @param lines  - the arguments -line[0] -the main folder - lines[1]- input file  -lines[2]- output file
 * @param fdResults - fd of the results file
 * @param workDir - the path of the run of this program
 * @return 1 if found c file - 0 oterwise
 */
int SearchIn(char *name, char lines[3][MAX_LINE], int fdResults, char *workDir) {
    DIR *pDir;
    struct dirent *pDirent;

    if ((pDir = opendir(name)) == NULL) {
        WRITE_ERROR
    }

    int seccses = chdir(name);
    if (seccses == -1) {
        WRITE_ERROR
    }

    // looping through the directory, printing the directory entry name
    while ((pDirent = readdir(pDir)) != NULL) {
        if (strcmp(pDirent->d_name, ".") == 0 || strcmp(pDirent->d_name, "..") == 0) {
            continue;
        }
        if (pDirent->d_type == 4) {
            if (SearchIn(pDirent->d_name, lines, fdResults, workDir)) {
                closedir(pDir);
                seccses = chdir("..");
                if (seccses == -1) {
                    WRITE_ERROR
                }
                return 1;
            }
        } else {
            if (strlen(pDirent->d_name) > 0 && pDirent->d_name[strlen(pDirent->d_name) - 1] == 'c'
                && pDirent->d_name[strlen(pDirent->d_name) - 2] == '.') {

                FindC(pDirent->d_name, lines, fdResults, workDir);
                closedir(pDir);
                seccses = chdir("..");
                if (seccses == -1) {
                    WRITE_ERROR
                }
                return 1;

            }
        }

    }

    closedir(pDir);

    seccses = chdir("..");
    if (seccses == -1) {
        WRITE_ERROR
    }
    return 0;

}

/**
 * over all the folder in the main folder and call SearcIn -if return 0 print NO_C_FILE
 * @param name  the main folder
 * @param lines  - the arguments -line[0] -the main folder - lines[1]- input file  -lines[2]- output file
 * @param fdResults
 * @param workDir
 */
void Search(char *name, char lines[3][MAX_LINE], int fdResults, char *workDir) {
    DIR *pDir;
    struct dirent *pDirent;


    if ((pDir = opendir(name)) == NULL) {
        WRITE_ERROR
    }

    int seccses = chdir(name);
    if (seccses == -1) {
        WRITE_ERROR
    }

    // looping through the directory, printing the directory entry name
    while ((pDirent = readdir(pDir)) != NULL) {
        if (strcmp(pDirent->d_name, ".") == 0 || strcmp(pDirent->d_name, "..") == 0) {
            continue;
        }
        if (pDirent->d_type == 4) {

            if (write(fdResults, pDirent->d_name, strlen(pDirent->d_name)) == -1) {
                WRITE_ERROR
            }

            if (!SearchIn(pDirent->d_name, lines, fdResults, workDir)) {
                NO_C_FILE
            }
        }

    }


    closedir(pDir);

    seccses = chdir("..");
    if (seccses == -1) {
        WRITE_ERROR
    }

}

/**
 *
 * @param file - input file - from args
 * @param lins - the arguments -line[0] -the main folder - lines[1]- input file  -lines[2]- output file
 */
void GetInput(char *file, char lins[3][MAX_LINE]) {
    int fd;
    char buffer;
    int i = 0, j = 0;
    ssize_t read1;

    if (access(file, F_OK) == 0) {
        fd = open(file, O_RDONLY);
        if (fd == -1) {
            WRITE_ERROR
        }
    } else {
        WRITE_ERROR
    }

    while (read1 = read(fd, &buffer, 1) != 0) {
        if (read1 == -1) {
            WRITE_ERROR
        }
        if (buffer == '\n') {
            i = 0;
            j++;
            continue;
        }
        if (j > 3) {
            WRITE_ERROR
        }
        lins[j][i] = buffer;
        i++;
    }
    close(fd);
}

