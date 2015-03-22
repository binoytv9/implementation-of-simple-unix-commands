#include<stdio.h>
#include<fcntl.h>
#include<errno.h>
#include<unistd.h>
#include<stdarg.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/types.h>

#define BUFMAX 1024

int openSrc(char *argv[]);
int openDes(char *argv[]);
void usageErr(char *prgName);
void errExit(char *errMsg, ...);

main(int argc, char *argv[])
{

    int sfd, dfd;
    char buf[BUFMAX];
    ssize_t numRead, numWrite;

    if(argc != 3 || (argc == 2 && !strcmp(argv[1], "--help")))
        usageErr(argv[0]);

    sfd = openSrc(argv);

    if((dfd = open(argv[2], O_CREAT | O_WRONLY | O_TRUNC, 0666)) == -1){
        if(errno == EISDIR)
            dfd = openDes(argv);
        else
            errExit("open '%s'", argv[2]);
    }

    while((numRead = read(sfd, buf, BUFMAX)) != 0){
        if(numRead == -1)
            errExit("read: '%s'", argv[1]);

        if((numWrite = write(dfd, buf, numRead)) == -1)
            errExit("write: '%s'", argv[2]);
    }

    close(sfd);
    close(dfd);

    exit(EXIT_SUCCESS);
}

int openSrc(char *argv[])
{
    int sfd;
    struct stat sbuf;

    if((sfd = open(argv[1], O_RDONLY)) == -1)
        errExit("open '%s'", argv[1]);

    if(fstat(sfd, &sbuf) == -1)
        errExit("stat: %s", argv[1]);

    if(sbuf.st_mode & S_IFDIR){
        fprintf(stderr, "%s: omitting directory '%s'\n", argv[0], argv[1]);
        exit(EXIT_FAILURE);
    }

    return sfd;
}

int openDes(char *argv[])
{
    int dfd;
    struct stat sbuf;
    char filename[BUFMAX];

    if(stat(argv[2], &sbuf) == -1)
        errExit("stat: %s", argv[2]);

    strcpy(filename, argv[2]);
    if(sbuf.st_mode & S_IFDIR){
        if(filename[strlen(filename)-1] != '/')
            strcat(filename, "/");
        strcat(filename, argv[1]);
    }
    if((dfd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0666)) == -1)
        errExit("open '%s'", filename);

    return dfd;
}

void usageErr(char *prgName)
{
    fprintf(stderr, "Usage: %s [SRC_FILE] [DEST_FILE]\n", prgName);
    exit(EXIT_FAILURE);
}

void errExit(char *errMsg, ...)
{
    va_list ap;

    va_start(ap, errMsg);
    vwarn(errMsg, ap);
    exit(EXIT_FAILURE);
}
