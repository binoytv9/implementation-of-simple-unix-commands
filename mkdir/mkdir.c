#include<stdio.h>
#include<stdarg.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<sys/types.h>

void usageErr(char *prgName);
void errExit(char *errMsg, ...);

int main(int argc, char *argv[])
{
    if(argc != 2 || (argc == 2 && !strcmp(argv[1], "--help")))
        usageErr(argv[0]);

    if(mkdir(argv[1], 0777) == -1)
        errExit("cannot create directory ‘%s’", argv[1]);

    exit(EXIT_SUCCESS);
}

void usageErr(char *prgName)
{
    fprintf(stderr, "Usage: %s [--help] DIR_NAME\n", prgName);
    exit(EXIT_FAILURE);
}

void errExit(char *errMsg, ...)
{
    va_list ap;

    va_start(ap, errMsg);
    vwarn(errMsg, ap);
    exit(EXIT_FAILURE);
}
