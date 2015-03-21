#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdlib.h>
#include<stdarg.h>
#include<sys/stat.h>
#include<sys/types.h>

void usageErr(char *prgName);
void errExit(char *errMsg, ...);

int main(int argc, char *argv[])
{
    int i;
    struct stat buf;

    if(argc < 2)
        usageErr(argv[0]);

    for(i = 1; argv[i] != NULL; ++i)
        if(unlinkat(AT_FDCWD, argv[i], 0) == -1)
            errExit("cannot remove '%s'", argv[i]);

    return 0;
}

void usageErr(char *prgName)
{
    fprintf(stderr, "Usage: %s [FILE]...\n", prgName);
    exit(EXIT_FAILURE);
}

void errExit(char *errMsg, ...)
{
    va_list ap;

    va_start(ap, errMsg);
    vwarn(errMsg, ap);
    exit(EXIT_FAILURE);
}