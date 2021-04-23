#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<unistd.h>

#define MAXARGS 128
#define MAXLINE 128

int builtin_command(char **argv)
{
    if (!strcmp(argv[0], "&"))      // Ignore singleton &
        return 1;
    return 0;                       // Not a builtin command
}

int parseline(char *buf, char **argv)
{
    char *delim;    // Points to first space delimiter
    int argc;
    int bg;

    buf[strlen(buf)] = ' ';
    buf[strlen(buf) + 1] = '\0';

    while (*buf && (*buf == ' '))   // Ignore leading spaces
        buf++;
    
    // Build the argv list
    argc = 0;
    while ((delim = strchr(buf, ' ')))
    {
        argv[argc++] = buf;
        *delim = '\0';
        buf = delim + 1;
        while (*buf && (*buf == ' '))   //Ignore spaces
            buf++;
    }
    argv[argc] = NULL;
    
    if (argc == 0)
        return 1;   // Ignore blank line

    // Should the job run in the background?
    if ((bg = (*argv[argc - 1] == '&')) != 0)
        argv[--argc] = NULL;

    return bg;
}

void mysys(char *command)
{
    char *argv[MAXARGS];    // Argument list execve()
    char buf[MAXLINE];      // Holds modified command line
    pid_t pid;              // Process id
    int bg;                 // Should the job run in bg or fg?

    strcpy(buf, command);   
    bg = parseline(buf, argv);

    if (argv[0] == NULL)
        return ;    // Ignore empty lines

    if (!builtin_command(argv))
    {
        pid = fork();
        if (pid < 0)
            printf("fork error");
        else if (pid == 0)
        {
            int error = execvp(argv[0], argv);
            if (error < 0)
            {
                printf("%s: Command not found.\n", argv[0]);
                exit(0);
            }
        }

        // Parent waits for foreground job to terminate
        if (!bg)
        {
            int status;
            if (wait(&status) < 0)
            {
               printf("waitfg: waitpid error");
               exit(0);
            }
        }
        else
            printf("%d %s\n", pid, command);
    }
    return ;
}

int main()
{
    printf("--------------------------------------------------\n");
    mysys("   echo     HELLO     WORLD");
    printf("--------------------------------------------------\n");
    mysys("ls /");
    printf("--------------------------------------------------\n");
    mysys("ls / &");
    printf("--------------------------------------------------\n");
    return 0;
}
