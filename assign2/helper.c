//
// Created by vishal on 16/01/20.
//

#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
    char shellpath[100] = "out";
    if(argc==2) strcpy(shellpath, argv[1]);
    char* arr[]={shellpath, "NULL"};
    pid_t child;
    child = fork();
    if(child<0) printf("Could not fork\n");
    if(child==0)
    {
        execvp(shellpath, arr);
        printf("Could not execute shell\n");
        exit(-1);
    } else wait(NULL);
    return 0;
}


