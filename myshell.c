
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <sys/wait.h> 
#include <fcntl.h> 

#define MAX_CMDLINE_LEN 256

#define MAX_ARGUMENTS 8

#define SPACE_CHARS " \t"

#define PIPE_CHAR "|"

#define MAX_PIPE_SEGMENTS 8

#define MAX_ARGUMENTS_PER_SEGMENT 9

#define STDIN_FILENO    0       
#define STDOUT_FILENO   1       

#define TEMPLATE_MYSHELL_START "Myshell (pid=%d) starts\n"
#define TEMPLATE_MYSHELL_END "Myshell (pid=%d) ends\n"

void process_cmd(char *cmdline);

void show_prompt();

int get_cmd_line(char *cmdline);


void read_tokens(char **argv, char *line, int *numTokens, char *token);


int main()
{
    char cmdline[MAX_CMDLINE_LEN];
    printf(TEMPLATE_MYSHELL_START, getpid());

    while (1)
    {
        show_prompt();
        if (get_cmd_line(cmdline) == -1)
            continue; /* empty line handling */

        if ( strcmp(cmdline, "exit") == 0 ) {
            printf(TEMPLATE_MYSHELL_END, getpid());
            exit(0);
        }

        pid_t pid = fork();
        if (pid == 0) {
            process_cmd(cmdline);
            exit(0); 
        } else {
            wait(0);
        }
            
    }
    return 0;
}

void process_cmd(char *cmdline)
{
    char command[MAX_CMDLINE_LEN];
    char firstArg[MAX_CMDLINE_LEN];
    char* args[MAX_ARGUMENTS_PER_SEGMENT];
    char *pipe_segments[MAX_PIPE_SEGMENTS];
    int pfds[2];
    pid_t pid = getpid();
    int num_pipe_segments = 1;
    int i = 0;
    int fd;

    while(cmdline[i]!='\0'){
        if(cmdline[i] == '|')
            num_pipe_segments++;
        i++;
    }

    read_tokens(pipe_segments, cmdline, &num_pipe_segments, "|");

    for(int pipeIteration = 0; pipeIteration < num_pipe_segments; pipeIteration++)
    {
        if(pid==0){
            i=0;
            while(pipe_segments[pipeIteration-1][i]!='\0'){
                command[i] = pipe_segments[pipeIteration-1][i];
                i++;
            }
            command[i] = '\0';
            close(1);
            dup2(pfds[1], 1);
            close(pfds[0]);
            break;
        }
        else{
            if(num_pipe_segments>1)
            {
                pipe(pfds);
                pid = fork();
                if(pid>0)
                {
                    close(0);
                    dup2(pfds[0], 0);
                    close(pfds[1]);
                    wait(0);
                    if(pipeIteration == num_pipe_segments-2)
                        break;
                    
                }
            }
        }
    }
    if(pid>0){
        i=0;
        while(pipe_segments[num_pipe_segments-1][i]!='\0'){
            command[i] = pipe_segments[num_pipe_segments-1][i];
            i++;
        }
        command[i] = '\0';
    }
    i = 0;
    int counter = 0;
    if(command[0]!=' ' || command[0]!='\t')
        counter = 1;
    while(command[i]!='\0'){
        if((command[i]==' ' || command[i]=='\t') && (command[i+1]!=' ' && command[i+1]!='\t' && command[i+1]!='\0'))
            counter++;
        i++;
    }

    read_tokens(args, command, &counter, SPACE_CHARS);
    args[counter] = NULL;

    i = 0;
    while (args[0][i] != '\0'){
        firstArg[i] = args[0][i];
        i++;
    }
    firstArg[i] = '\0';

    for(int i = 0; i < counter; i++){
        if(strcmp(args[i], "<") == 0){
            args[i] = NULL;
            fd = open(args[i+1], O_RDONLY, S_IRUSR | S_IWUSR);
            close(0);
            dup2(fd, 0);
        }
        else if(strcmp(args[i], ">") == 0){
            args[i] = NULL;
            fd = open(args[i+1], O_CREAT | O_WRONLY , S_IRUSR | S_IWUSR ); 
            close(1);
            dup2(fd, 1);
        }
    }

    execvp(args[0], args);
}

void read_tokens(char **argv, char *line, int *numTokens, char *delimiter)
{
    int argc = 0;
    char *token = strtok(line, delimiter);
    while (token != NULL)
    {
        argv[argc++] = token;
        token = strtok(NULL, delimiter);
    }
    *numTokens = argc;
}

void show_prompt()
{
    printf("Shell> ");
}

int get_cmd_line(char *cmdline)
{
    int i;
    int n;
    if (!fgets(cmdline, MAX_CMDLINE_LEN, stdin))
        return -1;
    n = strlen(cmdline);
    cmdline[--n] = '\0';
    i = 0;
    while (i < n && cmdline[i] == ' ') {
        ++i;
    }
    if (i == n) {
        return -1;
    }
    return 0;
}