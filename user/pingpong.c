#include "kernel/types.h"
#include "user/user.h"

enum PIPE_END { REC = 0, SND = 1 };

int main(int argc, char* argv[]) 
{
    if(argc != 1)
    {
        fprintf(2, "usage: pingpong (no parameter)");
        exit(114514);
    }
    int pid;
    int pipe1[2], pipe2[2];
    char buf[1] = "a";
    pipe(pipe1);
    pipe(pipe2);

    int ret = fork();

    if(ret == 0)
    {
        pid = getpid();
        close(pipe1[SND]);
        close(pipe2[REC]);
        if(read(pipe1[REC], buf, sizeof(buf)) > 0)
        {
            printf("%d: received ping\n", pid);
        }
        if(write(pipe2[SND], buf, sizeof(buf)) > 0)
        {
            // printf("%d: sent \n", pid);

        }
        exit(0);
    }
    else
    {
        pid = getpid();
        close(pipe2[SND]);
        close(pipe1[REC]);
        if(write(pipe1[SND], buf, sizeof(buf)) > 0)
        {
            // printf("%d: sent \n", pid);

        }
        if(read(pipe2[REC], buf, sizeof(buf)) > 0)
        {
            printf("%d: received pong\n", pid);
        }
        exit(0);
    }
}