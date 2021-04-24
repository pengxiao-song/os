#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<unistd.h>

#define MAXARGS 128
#define MAXLINE 128

void eval(char *command);
int parseline(char *buf, char **argv);
int builtin_command(char **argv);

int main()
{
    char command[MAXLINE];

    while (1)
    {
        printf("> ");
        
        fgets(command, MAXLINE, stdin);
        if (feof(stdin))
            exit(0);
        
        eval(command);
    }
}

void eval(char *command)
{
    char *argv[MAXARGS];
    char buf[MAXLINE];
    int bg;
    pid_t pid;

    strcpy(buf, command);
    bg = parseline(buf, argv);

    if (argv[0] == NULL)
        return ;

    if (!builtin_command(argv))
    {
        pid = fork();
        if (pid < 0)
        {
            perror("fork error");
            exit(0);
        }
        else if (pid == 0)
        {
            int error = execvp(argv[0], argv);
            if (error < 0)
            {
                perror("execvp error");
                exit(0);
            }
        }
        if (!bg)
        {
            int status;
            wait(&status);
            if (status < 0)
            {
                perror("wait error");
                exit(0);
            }
        }
        else
            printf("%d %s", pid, command);
    }
    return ;
}

int builtin_command(char **argv)
{
    if (!strcmp(argv[0], "cd"))
    {
        chdir(argv[1]);
        return 1;
    }
    if (!strcmp(argv[0], "exit"))
        exit(0);
    if (!strcmp(argv[0], "&"))
        return 1;
    return 0;
}

int parseline(char *buf, char **argv)
{
    char *delim;
    int argc;
    int bg;

    buf[strlen(buf) - 1] = ' ';
    while (*buf && (*buf == ' '))
        buf++;

    argc = 0;
    while ((delim = strchr(buf, ' ')))
    {
        argv[argc++] = buf;
        *delim = '\0';
        buf = delim + 1;
        while (*buf && (*buf == ' '))
            buf++;
    }
    argv[argc] = NULL;

    if (argc == 0)
        return 1;

    if ((bg = (*argv[argc - 1] == '&')) != 0)
        argv[--argc] = NULL;

    return bg;
}
