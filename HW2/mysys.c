#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<unistd.h>

void mysys(char *command)
{
    //initialize
    int cnt = 0, i = 0, j = 0, len = strlen(command);
    
    char *argv[256];
    memset(argv, '\0', sizeof argv);

    //split
    while (i < len && j < len)
    {
        if (command[j] == ' ')
        {
            argv[cnt] = (char*)malloc(j - i + 1);
            memcpy(argv[cnt++], command + i, j - i);
            while (command[j] == ' ')   j++;
            i = j;
        }
        j++;
    }    
    
    argv[cnt] = (char*)malloc(j - i + 1);
    memcpy(argv[cnt++], command + i, j - i);
   
    // fork and exec 
    pid_t pid = fork();
    if (pid == 0)
    {
        int error = execvp(argv[0], argv);
        if (error < 0)
            perror("execv");
    }
    int status;
    wait(&status);
}

int main()
{
    printf("--------------------------------------------------\n");
    mysys("echo     HELLO     WORLD");
    printf("--------------------------------------------------\n");
    mysys("ls /");
    printf("--------------------------------------------------\n");
    return 0;
}
