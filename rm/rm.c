#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdlib.h>
#include<stdarg.h>

void usageErr(char *prgName);
void errExit(char *errMsg, ...);

int main(int argc, char *argv[])
{
    int i;

    if(argc < 2 || (argc == 2 && !strcmp(argv[1], "--help")))
        usageErr(argv[0]);

    for(i = 1; argv[i] != NULL; ++i)
        if(unlinkat(AT_FDCWD, argv[i], 0) == -1)
            errExit("cannot remove '%s'", argv[i]);

    exit(EXIT_SUCCESS);
}

void usageErr(char *prgName)
{
    fprintf(stderr, "Usage: %s [--help] FILE [...]\n", prgName);
    exit(EXIT_FAILURE);
}

void errExit(char *errMsg, ...)
{
    va_list ap;

    va_start(ap, errMsg);
    vwarn(errMsg, ap);
    exit(EXIT_FAILURE);
}
