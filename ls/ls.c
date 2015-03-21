#include<pwd.h>
#include<grp.h>
#include<time.h>
#include<errno.h>
#include<stdio.h>
#include<stdarg.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<dirent.h>
#include<limits.h>
#include<stdbool.h>
#include<sys/stat.h>
#include<sys/types.h>

void usageErr(char *prgName);
void errExit(char *errMsg, ...);
void printTime(struct stat *buf);
void printFileType(struct stat *buf);
void printLongFormat(char *filename);
void printUsrGrpName(struct stat *buf);
void printFilePermission(struct stat *buf);
void print(char *filename, int l_flag, int r_flag);
void resolvePath(char *fullPath, char *filename, struct dirent *dp);
void parseArgv(char *argv[], char **filename, int *l_flag, int *r_flag);

int main(int argc, char *argv[])
{
    int l_flag = false; // use a long listing format
    int r_flag = false; // list subdirectories recursively

    char *filename = ".";   // default filename

    parseArgv(argv, &filename, &l_flag, &r_flag);
    print(filename, l_flag, r_flag);
 
    printf("\n");
    return 0;
}

// extract the filename and other flags
void parseArgv(char *argv[], char **filename, int *l_flag, int *r_flag)
{
    char opt;
    char *prgName = argv[0];

    while(*++argv != NULL)
        if(*argv[0] == '-')
            while((opt = *++argv[0]) != '\0')
                switch(opt){
                    case 'l':
                        *l_flag = true;;
                        break;
                    case 'R':
                        *r_flag = true;
                        break;
                    default:
                        fprintf(stderr, "%s: invalid option -- '%c'\n", prgName, opt);
                        usageErr(prgName);
                        break;
                }
        else
            *filename = *argv;
}

// print the content of a directory
void print(char *filename, int l_flag, int r_flag)
{
    DIR *dirp;
    char *bufp;
    struct dirent *dp;
    char buf[PATH_MAX + 1];
    char fullPath[PATH_MAX + 1] = "";

    if((dirp = opendir(filename)) == NULL)
        errExit("opendir: '%s'", filename);

    if(r_flag)
        printf("%s:\n", filename);
    while(errno = 0, (dp = readdir(dirp)) != NULL){
        if(l_flag){ // display long listing format
            resolvePath(fullPath, filename, dp);
            if((bufp = realpath(fullPath, buf)) == NULL)
                errExit("realpath %s", fullPath);

            printLongFormat(bufp);
            printf(" %s\n", dp->d_name);
        }
        else // just print the file name
            printf("%s\t", dp->d_name);

        // if recursive, if directory , if name not "." , if name not ".."
        if(r_flag && (dp->d_type & DT_DIR) && strcmp(dp->d_name, ".") && strcmp(dp->d_name, ".."))
            print(fullPath, l_flag, r_flag);
    }
    if(errno != 0)
        errExit("readdir: '%s'", filename);

    if(closedir(dirp) == -1)
        errExit("closedir: '%s'", filename);
}

// append filename with basename
void resolvePath(char *fullPath, char *filename, struct dirent *dp)
{
    strcpy(fullPath, filename);
    if(filename[strlen(filename)-1] != '/')
        strcat(fullPath, "/");
    strcat(fullPath, dp->d_name);
}

// print files as long listing format
void printLongFormat(char *filename)
{
    struct stat buf;

    if(stat(filename, &buf) == -1)
        errExit("stat: '%s'", filename);

    printFileType(&buf);
    printFilePermission(&buf);
    printf(" %ld", (long)buf.st_nlink);
    printUsrGrpName(&buf);
    printf(" %ld", (long)buf.st_size);
    printTime(&buf);
}

// prints the file type
void printFileType(struct stat *buf)
{
    switch(buf->st_mode & S_IFMT){
        case S_IFDIR    : printf("d"); break;
        case S_IFIFO    : printf("f"); break;
        case S_IFBLK    : printf("b"); break;
        case S_IFCHR    : printf("c"); break;
        case S_IFLNK    : printf("l"); break;
        case S_IFREG    : printf("-"); break;
        case S_IFSOCK   : printf("s"); break;
        default         : printf("?"); break;
    }
}

// prints the permissions on a file
void printFilePermission(struct stat *buf)
{
    char per[10] = "---------";

    // user
    if(buf->st_mode & S_IRUSR) per[0] = 'r';
    if(buf->st_mode & S_IWUSR) per[1] = 'w';
    if(buf->st_mode & S_IXUSR) per[2] = 'x';

    //group
    if(buf->st_mode & S_IRGRP) per[3] = 'r';
    if(buf->st_mode & S_IWGRP) per[4] = 'w';
    if(buf->st_mode & S_IXGRP) per[5] = 'x';

    //others
    if(buf->st_mode & S_IROTH) per[6] = 'r';
    if(buf->st_mode & S_IWOTH) per[7] = 'w';
    if(buf->st_mode & S_IXOTH) per[8] = 'x';

    printf("%s", per);
}

// print the owner and group name for a file
void printUsrGrpName(struct stat *buf)
{
    struct passwd *up;
    struct group *gp;

    errno = 0;
    if((up = getpwuid(buf->st_uid)) == NULL)
        errExit("getpwuid");

    errno = 0;
    if((gp = getgrgid(buf->st_gid)) == NULL)
        errExit("getgrgid");

    printf(" %s", up->pw_name);
    printf(" %s", gp->gr_name);
}

// print the last modified time of a file
void printTime(struct stat *buf)
{
    char *time;

    if((time = ctime(&buf->st_mtime)) == NULL)
        errExit("ctime");

    time[strlen(time) - 1] = '\0';
    printf("\t%s", time);
}

void usageErr(char *prgName)
{
    fprintf(stderr, "Usage: %s [OPTION]... [FILE]\n"
                    "-l\t use a long listing format\n"
                    "-R\t list subdirectories recursively\n", prgName);
    exit(EXIT_FAILURE);
}

void errExit(char *errMsg, ...)
{
    va_list ap;

    va_start(ap, errMsg);
    vwarn(errMsg, ap);
    exit(EXIT_FAILURE);
}
