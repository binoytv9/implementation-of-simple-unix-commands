#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include<unistd.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/types.h>

#define MAXIN 1024

void usageErr(char *prgName);
void errExit(char *errMsg, ...);
void write2all(const char *input, int numRead, int fd[], char *argv[]);

int main(int argc, char *argv[])
{
    int i;
    int numRead;
    int fd[argc];
    char input[MAXIN];

    if(--argc && strcmp(argv[1], "--help") == 0)
        usageErr(argv[0]);

    for(i = 0; argv[i+1] != NULL; ++i){
        if((fd[i] = open(argv[i+1],  O_WRONLY | O_CREAT | O_TRUNC, 0666)) == -1)
            errExit("open: %s", argv[i+1]);
    }

    while(fgets(input, MAXIN, stdin) != NULL){
        numRead = strlen(input);
        if(write(1, input, numRead) == -1)
            errExit("write: stdout");
        if(argc)
            write2all(input, numRead, fd, argv);
    }

    for(i = 0; argv[i+1] != NULL; ++i)
        if(close(fd[i]) == -1)
            errExit("close: %s", argv[i+1]);

    exit(EXIT_SUCCESS);
}

void write2all(const char *input, int numRead, int fd[], char *argv[])
{
    int i;

    for(i = 0; argv[i+1] != NULL; ++i)
        if(write(fd[i], input, numRead) == -1)
            errExit("write: %s", argv[i+1]);
}

void usageErr(char *prgName)
{
    fprintf(stderr, "Usage: %s [FILE]...\n"
                    "Copy standard input to each FILE, and also to standard output\n"
                  , prgName);

    exit(EXIT_FAILURE);
}

void errExit(char *errMsg, ...)
{
    va_list ap;

    va_start(ap, errMsg);
    vwarn(errMsg, ap);
    exit(EXIT_FAILURE);
}
