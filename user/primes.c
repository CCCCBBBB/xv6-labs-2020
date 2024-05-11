#include "kernel/types.h"
#include "user/user.h"


void recursion(int inputfd)
{
    int prime = 0;
    int inputNum = 0;
    int forked = 0;

    int pipes[2];
    while(1)    
    {
        int readByte = read(inputfd, &inputNum, sizeof(int));//读取管道中的int
        if(readByte == 0) // 为 0 说明已经没有数据且管道的写端已关闭
        {
            close(inputfd);
            if(forked)
            {
                close(pipes[1]);
                wait(0);
            }
            exit(0);
        }

        if(prime == 0) //收到的首个数字即为质数
        {
            prime = inputNum;
            printf("prime %d\n", prime);
            // continue;
        }

        if(inputNum % prime != 0) //不为本轮质数的倍数就传给下一个子进程
        {
            if(!forked)
            {
                forked = 1;
                pipe(pipes);

                int ret = fork();
                if(ret == 0)
                {
                    close(pipes[1]); //关闭写端
                    close(inputfd); //记得关闭本进程的inputfd
                    recursion(pipes[0]);
                }
                else
                {
                    close(pipes[0]); //关闭读端
                }
            }

            write(pipes[1], &inputNum, sizeof(int));
        }

    }

}

int main(int argc, char* argv[])
{   
    int pipes[2];
    pipe(pipes);
    for(int i = 2; i <= 35; i++)
    {
        write(pipes[1], &i, sizeof(int));
    }
    close(pipes[1]);
    recursion(pipes[0]);
    exit(0);
}

