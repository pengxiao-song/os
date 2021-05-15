#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#define MAXARGS 128
#define MAXLINE 128

void eval(char *command);
int parseline(char *buf, char **argv);
int builtin_command(char **argv);


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

    for (int i = 0; i < MAXARGS; i++)	
    	argv[i] = NULL;
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
        if (chdir(argv[1]) == -1)
            perror("cd error");
        return 1;
    }
    if (!strcmp(argv[0], "pwd"))
    {
        char cwd[MAXLINE];
        if (!getcwd(cwd, MAXLINE))
            perror("pwd error");
        printf("%s\n", cwd);
        return 1;
    }
    if (!strcmp(argv[0], "echo"))
    {
    	int fd;
    	int idx;
    	int tmp;

    	for (idx = 0; idx < MAXARGS; idx++)
    		if (argv[idx] && !strcmp(argv[idx], ">"))	break;
    	
    	if (idx != MAXARGS)
    	{
    		if (!argv[idx + 1])	{
    			perror("bash: syntax error near unexpected token 'newline'");
    			return 1;
    		}
    		fd = open(argv[idx + 1], O_CREAT | O_RDWR, 0666);
    		tmp = dup(1);
    		dup2(fd, 1);
    		close(fd);
    	}
    	for (int i = 1; i < idx; i++){
    		if (argv[i]){
    			write(1, argv[i], strlen(argv[i]));
    			if (i != idx - 1)	write(1, " ", strlen(" "));
    		}	
    	}
    	write(1, "\n", strlen("\n"));
    	dup2(tmp, 1);
    	close(tmp);
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
