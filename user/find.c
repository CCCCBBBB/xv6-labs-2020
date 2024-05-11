#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "kernel/fs.h"
#include "kernel/stat.h"
#include "user/user.h"

char* getFileName(char* path) //获取文件名
{
    char* ptr = path + strlen(path);
    for(; ptr >= path && *ptr != '/'; ptr--) {}
    return ++ptr;
}

void find(char* curpath, char* target)
{
    int fd;
    struct dirent de;
    struct stat st;
    char buf[256] = {0};
    if ((fd = open(curpath, O_RDONLY)) < 0) //打开目录
    {
        fprintf(2, "find: cannot open %s\n", curpath);
        return;
    }

    if(fstat(fd, &st) < 0) //判断目录还是文件
    {
        fprintf(2, "find: cannot stat %s\n", curpath);
        close(fd);
        return;        
    }

    switch (st.type)
    {
    case T_FILE:
        char* filename = getFileName(curpath);
        if(strcmp(filename, target) == 0) //等于返回0
        {
            printf("%s\n", curpath);
        } 
        close(fd);
        break;
    case T_DIR:
        memset(buf, 0, sizeof(buf));
        uint curpathlen = strlen(curpath);
        memmove(buf, curpath, curpathlen);
        buf[curpathlen] = '/';
        char* p = buf + curpathlen + 1;//p指向目录末尾后一位  用来添加新的文件/目录名
        while(read(fd, &de, sizeof(de)) == sizeof(de))//目录文件是一个字节数组，而里面每16字节都记录了一个文件名，
        {
            if (de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0 ) continue;
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            find(buf, target);
        }
        close(fd);
        break;
    default:
        break;
    }
}

int main(int argc, char* argv[])
{
    if (argc != 3) 
    {
        fprintf(2, "usage: find [directory] [target filename]\n");
        exit(1);
    }
    find(argv[1], argv[2]);
    exit(0);  
}