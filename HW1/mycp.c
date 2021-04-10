#include<stdio.h>
#include<stdbool.h>
#include<sys/stat.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>

int main(int argc, char *argv[])
{
    if (argc == 2)
    {
        printf("cp: missing destination file operand after '%s'\n", argv[1]);
        exit(0);
    }

    if (argc > 3)
    {
        printf("cp: target '%s' is not a directory\n", argv[3]);
        exit(0);
    }
    
    // get fd
    mode_t mode = 0777;
   
    int sfd = open(argv[1], O_RDONLY, mode);
    if (sfd < 0)
    {
        printf("cp: cannot stat '%s': No such file or directory\n", argv[1]);
        exit(0);
    }
    int tfd = open(argv[2], O_CREAT | O_WRONLY | O_TRUNC, mode);
    if (tfd < 0)
    {
        printf("something wrong when opening...\n");
        exit(0);
    }

    char buf[128];
    while (true)
    {
        int rdcnt = read(sfd, buf, sizeof(buf));
        if (rdcnt == -1)
        {
            printf("something wrong when reading...\n");
            exit(0);
        }
        else if (rdcnt == 0)
        {
            break;
        }
        else
        {
            int wrcnt = write(tfd, buf, rdcnt);
            if (wrcnt == -1)
            {
                printf("something wrong when writing...\n");
                exit(0);
            }
        }
    }

    //close file
    close(sfd);
    close(tfd);

    return 0;
}
