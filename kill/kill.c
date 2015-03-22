#include<stdio.h>
#include<errno.h>
#include<stdlib.h>
#include<signal.h>
#include<stdarg.h>
#include<sys/types.h>

void usageErr(char *prgName);
void errExit(char *errMsg, ...);

int main(int argc, char *argv[])
{
    int pid;
    int flag = 0;
    char *pName;
    int sig = SIGTERM; // default

    pName = argv[0];
    if(argc < 2 || (argc == 2 && !strcmp(argv[1], "--help")))
        usageErr(pName);

    while((++argv)[0] != NULL)
        if(*argv[0] == '-')
            sig = atoi(++(argv[0]));
        else{
            pid = atoi(argv[0]);
            flag = 1;
        }

    if(!flag)
        usageErr(pName);

    if(kill(pid, sig) == -1)
        switch(errno){
            case EINVAL:
                errExit("%d", sig);
                break;
            case ESRCH:
                errExit("(%d)", pid);
                break;
            default:
                errExit(NULL);
                break;
        }

    exit(EXIT_SUCCESS);
}

void usageErr(char *prgName)
{
    fprintf(stderr, "Usage: %s SIG PID\n", prgName);
    exit(EXIT_FAILURE);
}

void errExit(char *errMsg, ...)
{
    va_list ap;

    va_start(ap, errMsg);
    vwarn(errMsg, ap);
    exit(EXIT_FAILURE);
}
