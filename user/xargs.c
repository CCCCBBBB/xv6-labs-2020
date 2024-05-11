#include "kernel/param.h"
#include "kernel/types.h"
#include "user/user.h"

#define BUFSIZE 512

int main(int argc, char *argv[])
{
    char *xargv[MAXARG] = {0};
    for (int i = 1; i < argc; i++)
    {
        xargv[i - 1] = argv[i];
    }
    char buf[BUFSIZE + 1] = {0};
    uint readableByte = 0;
    int finish = 0;
    while (!(finish && readableByte == 0))
    {
        if (!finish)
        {
            int remainSize = BUFSIZE - readableByte;
            int readBytes = read(0, buf + readableByte, remainSize); // 读到标准输入
            printf("read once\n");
            if (readBytes < 0)
            {
                fprintf(2, "xargs: read returns -1 error\n");
            }
            if (readBytes == 0)
            {
                close(0);
                finish = 1;
            }
            readableByte += readBytes;
        }
        char *lineEnd = strchr(buf, '\n');
        while (lineEnd)
        {
            char tempBuf[BUFSIZE + 1] = {0};
            memmove(tempBuf, buf, lineEnd - buf);
            xargv[argc - 1] = tempBuf;

            int ret = fork();
            if (ret == 0)
            {
                if (!finish)
                {
                    close(0);
                }
                if (exec(argv[1], xargv) < 0)
                {
                    fprintf(2, "xargs: exec fails with -1\n");
                    exit(1);
                }
            }
            else
            {
                readableByte -= lineEnd - buf + 1;
                memmove(buf, lineEnd + 1, readableByte);
                memset(buf + readableByte, 0, BUFSIZE - readableByte);

                wait(0);
                printf("exe once\n");
                lineEnd = strchr(buf, '\n');
            }
        }
    }
    exit(0);
}