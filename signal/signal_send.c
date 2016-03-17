#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

int main()
{
        //打开管道，执行shell 命令
    FILE *fp = popen("ps -e | grep \'signal_recv\' | awk \'{print $1}\'", "r");    
    if(fp==NULL)
        return -1;
    char buffer[30] = {0};
    while (NULL != fgets(buffer, 30, fp)) //逐行读取执行结果并打印
    {
        printf("PID:%s\n", buffer);
        break;
    }
    pid_t target_pid=atoi(buffer);
    pclose(fp); //关闭返回的文件指针，注意不是用fclose噢
    union sigval mysigval;
    mysigval.sival_int = 100;
    printf("sending SIGIUSR1 signal to %d......\n",target_pid);
    if(sigqueue(target_pid,SIGUSR1,mysigval) == -1){
        perror("sigqueue error");
        exit(EXIT_FAILURE);
    }
    return 0;
}
