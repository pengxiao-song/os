#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#define MAXARGS     128
#define MAXLINE     128
#define MAXCOMANDS  128

typedef struct 
{
    int argc;
    char *argv[MAXARGS];
    char *input;
    char *output;
} command;

void eval(char *command);
void parse_commands(char *line);
void parse_command(char *field, struct command *cmd);
int builtin_command(char **argv);
void dump_commands(char *command)    //test

int command_count;
struct command commands[MAXCOMANDS];

void parse_commands(char *line)
{
    char *delim;
    char *fields[MAXCOMANDS];

    line[strlen(line) - 1] = '|';
    while (delim = strchr(line, '|')) {
        *delim = '\0';
        fields[command_count++] = line;
        line = delim + 1;
    }
    for (int i = 0; i < command_count; i++) {
        parse_command(fields[i], &commands[i]);
    }
    free(fields);
}

void dump_commands(char *command)
{
    for (int i = 0; i < command_count; i++) {
        int cnt = command[i].argc;
        for (int j = 0; j < cnt; j++) {
            printf("%s ", argv[j]);
        }
        puts("");
    }
}

void parse_command(char *field, struct command *cmd)
{
    char *delim;

    field[strlen(field) - 1] = ' ';
    while (*field == ' ')   field++;

    cmd->argc = 0;
    while (delim = strchr(field, ' ')) {
        cmd->argv[argc++] = field;
        *delim = '\0';
        field = delim + 1;
        while (*field == ' '))  field++;
    }
    cmd->argv[cmd->argc] = NULL;
}


int main()
{
    char line[MAXLINE];

    while (1)
    {
        printf("> ");
        
        fgets(line, MAXLINE, stdin);
        if (feof(stdin))
            exit(0);
        
        eval(line);
    }
}

void eval(char *line)
{
    pid_t pid;
    parse_commands(line);
    if (command_count == 1) {
        
        if (argv[0] == NULL)    return ;    
    }
    

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
            printf("%d %s", pid, line);
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
        
        if (idx != MAXARGS){
            dup2(tmp, 1);
            close(tmp);   
        }
    	return 1;
    }
    if (!strcmp(argv[0], "exit"))
        exit(0);
    if (!strcmp(argv[0], "&"))
        return 1;
    return 0;
}

