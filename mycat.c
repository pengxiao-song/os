#include<stdio.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<stdlib.h>

int main(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++)
    {
        // open the file
        int fd = open(argv[i], O_RDONLY);
        if (fd < 0)
        {
            printf("cat: %s: No such file or directory\n", argv[i]);
            exit(0);
        }
        
        // get file size
        struct stat buf;
        fstat(fd, &buf);
        int fsize = buf.st_size;

        // get file content 
        char *content = (char*)malloc(fsize * sizeof(char));
        read(fd, content, fsize);
        printf("%s", content);
        free(content);

        close(fd);
    }
    return 0;
}
