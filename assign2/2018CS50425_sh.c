//
// Created by vishal on 13/01/20.
//
#include <stdio.h>
#include "string.h"
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <fcntl.h>

#define max_limit 100
long max_path_size;

int word_array(char str[], char *array[]); // Puts array of words in variable array. Returns no of words
char *trim_ws(char *str); // trim trailing and leading whitespace, and newline character
void execute(char *str, char *arr[]);
int exec_cmd(char *str);
int pipe_parse(char *str, char *array[]);
int exec_pipe(char *str, char *arr[]);
int notreserved(char c);

int main() {
    max_path_size = pathconf(".", _PC_PATH_MAX);
    char str[max_limit];
    char *arr[max_limit];
    while (1) {
        printf("shell>");
        fgets(str, max_limit, stdin);
        strcpy(str, trim_ws(str));
        if (pipe_parse(str, arr) > 1) {
            if (exec_pipe(str, arr) == -1)
                printf("Pipe function failed!");
        } else if (exec_cmd(str) == -1)
            break;
    }
    return 0;
}

int redirect_parse(char* str, char* outpath, char* inpath, char* cmd)
{
    char inp[max_limit], outp[max_limit];
    inp[0]='\0';outp[0]='\0';
    int found1=0;
    for(int i=0;i<strlen(str);i++)
        if(str[i]=='<') {
            for (int j = i + 1; j < strlen(str); j++) {
                if(!found1 && str[j]==' ' )
                {
                    i++;
                    continue;
                }
                if (!notreserved(str[j]))
                {
                    inp[j-i-1]='\0';
                    break;
                }
                inp[j - i - 1] = str[j];
                found1=1;
            }
            break;
        }
    int found2=0;
    for(int i=0;i<strlen(str);i++)
        if(str[i]=='>') {
            for (int j = i + 1; j < strlen(str); j++) {
                if(!found2 && str[j]==' ')
                {
                    i++;
                    continue;
                }
                if (!notreserved(str[j]))
                {
                    outp[j-i-1]='\0';
                    break;
                }
                outp[j - i - 1] = str[j];
                found2=1;
            }
            break;
        }
    strcpy(inpath, inp);
    strcpy(outpath, outp);

    if(inp[0]!='\0' || outp[0]!='\0') {
        char strcopy[max_limit];
        strcpy(strcopy, str);
        strcpy(cmd, strtok(strcopy, "<>"));
        return 1;
    }
    cmd[0]='\0';
    return 0;
}

int notreserved(char c)
{
    char reserved[]={'>', '<', '|', ' ', '\n', '\0'};
    for(int i=0;i< sizeof(reserved);i++)
        if(c==reserved[i])
            return 0;
    return 1;
}
int exec_pipe(char *str, char **arr) {
    printf("--Executing built-in implementation of piping!--\n");
    pid_t pipe_process;
    pipe_process=fork();
    if(pipe_process<0) return -1;
    if(pipe_process==0) {
        int pipefd[2];
        pid_t child;
        if (pipe(pipefd) < 0) return -1;
        child = fork();
        if (child < 0) return -1;
        if (child == 0) {
            close(pipefd[0]);
            dup2(pipefd[1], STDOUT_FILENO);
            pipe_parse(str, arr);
            exec_cmd(arr[0]);
            exit(0);
        } else {
            wait(NULL);
            close(pipefd[1]);
            dup2(pipefd[0], STDIN_FILENO);
            pipe_parse(str, arr);
            exec_cmd(arr[1]);
            exit(0);
        }
    }
    else wait(NULL);
    return 0;
}

int exec_redirect(char* str, char* inp, char* outp)
{
    printf("--Executing built-in implementation of redirection!-- \n");
    pid_t child = fork();
    if(child<0) return -1;
    if(child==0)
    {
        if(inp[0]!='\0')
        {
            int fd1=open(inp, O_RDONLY);
            if(fd1==-1)
            {
                printf("Could not open file!\n");
                exit(-1);
            }
            dup2(fd1, STDIN_FILENO);
            close(fd1);
        }
        if(outp[0]!='\0')
        {
            int fd2=creat(outp, 0644);
            dup2(fd2, STDOUT_FILENO);
            close(fd2);
        }
        exec_cmd(str);
        exit(0);
    }
    else wait(NULL);
    return 0;
}

int exec_cmd(char *str) {
    char *arr[max_limit];
    int no_of_words = word_array(str, arr);
    char cur_dir[max_path_size];
    char inputpath[max_limit], outputpath[max_limit], cmdredir[max_limit];
    if(redirect_parse(str, outputpath, inputpath, cmdredir))
        return exec_redirect(cmdredir, inputpath, outputpath);
    if (strcmp(str, "") == 0) return 0;
    if (strcmp(arr[0], "pwd") == 0) {
        printf("--Executing built-in implementation of pwd--\n");
        getcwd(cur_dir, max_path_size);
        printf("%s\n", cur_dir);
    } else if (strcmp(arr[0], "cd") == 0) {
        printf("--Executing built-in implementation of cd--\n");
        if (no_of_words != 2)
            printf("Invalid syntax for cd\n");
        else {
            int ret = chdir(arr[1]);
            ret == 0 ? printf("Changed current directory to %s\n", getcwd(cur_dir, max_path_size))
                     : printf("Could not change directory\n");
        }
    } else if (strcmp(arr[0], "mkdir") == 0) {
        printf("--Executing built-in implementation of mkdir--\n");
        if (no_of_words != 2) {
            printf("Invalid syntax for mkdir\n");
            return 0;
        }
        int ret = mkdir(arr[1], 0777);
        ret == 0 ? printf("Created directory \"%s\" successfully\n", arr[1])
                 : printf("Unable to create directory\n");
    } else if (strcmp(arr[0], "rmdir") == 0) {
        printf("--Executing built-in implementation of rmdir--\n");
        if (no_of_words != 2) {
            printf("Invalid syntax for rmdir\n");
            return 0;
        }
        int ret = rmdir(arr[1]);
        ret == 0 ? printf("Removed directory \"%s\" successfully\n", arr[1])
                 : printf("Unable to remove directory\n");
    } else if (strcmp(arr[0], "exit") == 0) {
        printf("--Executing built-in implementation of exit--\n");
        if (no_of_words != 1)
            printf("Invalid syntax for exit\n");
        else
            return -1;
    } else {
        execute(str, arr);
    }
    return 0;
}

int pipe_parse(char *str, char *array[]) {
    char strcopy[max_limit];
    strcpy(strcopy, str);
    int i = 0;
    char *word = strtok(strcopy, "|");
    while (word != NULL && i < max_limit) {
        array[i] = word;
        word = strtok(NULL, "|");
        i++;
    }
    array[i] = NULL;
    return i;
}

int word_array(char *str, char *array[]) {
    char strcopy[max_limit];
    strcpy(strcopy, str);
    int i = 0;
    char *word = strtok(strcopy, " ");
    while (word != NULL && i < max_limit) {
        array[i] = word;
        word = strtok(NULL, " ");
        i++;
    }
    array[i] = NULL;
    return i;
}

char *trim_ws(char *str) {
    int i = 0;
    while (isspace((unsigned char) *str)) {
        i++;
        str++;
    }
    if (strlen(str) <= 2) {
        if (strlen(str) == 2) str[1] = '\0';
        else str[0] = '\0';
        return str;
    }
    char *end;
    end = str + strlen(str) - 2;
    while (isspace((unsigned char) *end)) end--;
    end[1] = '\0';
    return str;
}

void execute(char *str, char *arr[]) {
    pid_t child_pid;
    int child_status;
    child_pid = fork();
    if (child_pid == 0) {
        // child process
        if(execv(arr[0], arr)==-1) //arr[0] considered direct path
            execvp(arr[0], arr); // arr[0] searched in PATH
        // If execvp returns, it must have failed
        printf("\"%s\" is not a valid command\n", str);
        exit(0);
    } else {
        //  parent
        pid_t tpid;
        do {
            tpid = wait(&child_status);
        } while (tpid != child_pid);
    }
}