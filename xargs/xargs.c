#include<stdio.h>
#include<stdarg.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

#define MAXARG 50
#define MAXIN 1024

void usageErr(char *prgName);
void errExit(char *errMsg, ...);
char **strip(char *in, char **argp);

int main(int argc, char *argv[])
{
    char **argp;
    char input[MAXIN];
    char *args[MAXARG];

    if(--argc && !strcmp(argv[1], "--help"))
        usageErr(argv[0]);

    argp = args;
    while(*++argv != NULL)
        *argp++ = strdup(*argv);

    // default: echo
    if(argc == 0)
        *argp++ = strdup("echo");

    while(fgets(input, MAXIN, stdin) != NULL)
        argp = strip(input, argp);

    *argp = NULL;

    switch(fork()){
        case -1:
            errExit("fork");

        case 0:
            execvp(args[0], args);
            errExit("execvp");

        default:
            wait(NULL);
            argp = args;
            while(*argp != NULL)
                free(*argp++);

            exit(EXIT_SUCCESS);
    }
}

char **strip(char *in, char **argp)
{
    char *bufp;
    char buf[MAXIN];

    bufp = buf;
    while(*in != '\0'){
        while(*in != '\0' && *in != ' ' && *in != '\n')
            *bufp++ = *in++;

        *bufp = '\0';
        *argp++ = strdup(buf);
        bufp = buf;

        if(*in == ' ' || *in == '\n')
            *in++;
        else
            break;
    }

    return argp;
}

void usageErr(char *prgName)
{
    fprintf(stderr, "Usage: %s [--help] [command [initial-arguments]]\n", prgName);
    exit(EXIT_FAILURE);
}

void errExit(char *errMsg, ...)
{
    va_list ap;

    va_start(ap, errMsg);
    vwarn(errMsg, ap);
    exit(EXIT_FAILURE);
}
