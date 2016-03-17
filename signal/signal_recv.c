#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define ERR_EXIT(m) \
    do \
    { \
        perror(m); \
        exit(EXIT_FAILURE); \
    } while(0)

void my_sigaction(int sig,siginfo_t *info,void *ctx);

int main(int argc, char *argv[])
{
    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = my_sigaction;
    if (sigaction(SIGUSR1, &act, NULL) < 0)
        ERR_EXIT("sigaction error\n");

    for (;;)
        pause();
    return 0;
}

void my_sigaction(int sig,siginfo_t *info,void *ctx)
{
    printf("receive the data from siqueue by info->si_int is %d\n",info->si_int);
    printf("receive the data from siqueue by info->si_value.sival_int is %d\n",info->si_value.sival_int);
}
