#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>

#define BUFFER_SIZE 100
char buffer[BUFFER_SIZE];
int y,pid,num;
char chars;

void handleruser1child(int sig)
{

    if (chars == 't')
    {
        sprintf(buffer, "[ID=%d/PID=%d/TIME=%ld] The gates are open!\n", num, pid, time(NULL)-y);
        write(1, buffer, strlen(buffer));
    }
    else if (chars == 'f')
    {
        sprintf(buffer, "[ID=%d/PID=%d/TIME=%ld] The gates are closed!\n", num, pid, time(NULL)-y);
        write(1, buffer, strlen(buffer));
    }
}

void handleruser2child(int sig)
{

    if (chars == 't')
    {
        chars = 'f';
    }
    else if (chars == 'f')
    {
        chars = 't';
    }
}

void handleralarm(int sig)
{
    if (chars == 't')
    {
        sprintf(buffer, "[ID=%d/PID=%d/TIME=%ld] The gates are open!\n", num, pid, time(NULL)-y);
        write(1, buffer, strlen(buffer));
    }
    else if (chars == 'f')
    {
        sprintf(buffer, "[ID=%d/PID=%d/TIME=%ld] The gates are closed!\n", num, pid, time(NULL)-y);
        write(1, buffer, strlen(buffer));
    }
    alarm(15);
}
int main(int argc, char *argv[])
{
  num=atoi(argv[1]);
  pid=atoi(argv[2]);
  chars=*argv[3];
    y=time(NULL);
    struct sigaction actionuser1child;
    actionuser1child.sa_handler = handleruser1child;
    actionuser1child.sa_flags = SA_RESTART;
    sigaction(SIGUSR1, &actionuser1child, NULL);

    struct sigaction actionuser2child;
    actionuser2child.sa_handler = handleruser2child;
    actionuser2child.sa_flags = SA_RESTART;
    sigaction(SIGUSR2, &actionuser2child, NULL);

    struct sigaction actionalarm;
    actionalarm.sa_handler = handleralarm;
    actionalarm.sa_flags = SA_RESTART;
    sigaction(SIGALRM, &actionalarm, NULL);

    if (chars== 't')
    {
        printf("[ID=%d/PID=%d/TIME=%ld] The gates are open!\n", num, pid,time(NULL)-y);
    }
    else if (chars== 'f')
    {
        printf("[ID=%d/PID=%d/TIME=%ld] The gates are closed!\n", num, pid, time(NULL)-y);
    }
    alarm(15);
    while (1)
    {
    }
}
