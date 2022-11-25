#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define FD_STDIN 0
#define FD_STDOUT 1
#define FD_STDERR 2
#define BUFFER_SIZE 1000
#define PERMS 0644

int main(int argc, char *argv[])
{
        char message1[20];
        int n_read, n_write, status;
        int orisma = atoi(argv[2]);
        char buffer[BUFFER_SIZE];
        int i;
        int fd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, PERMS);
        if (fd == -1)
        {
                perror("open");
                exit(-1);
        }
        pid_t child_1 = fork();
        if (child_1 < 0)
        {
                perror("fork");
        }
        else if (child_1 == 0)
        {
                printf("[Child 1] Started. PID=%d PPID=%d\n", getpid(), getppid());
                sleep(1);
                for (i = 0; i < orisma + 1; i += 2)
                {
                        printf("[Child 1] Heartbeat. PID=%d Time=%ld x=%d\n", getpid(), time(NULL), i);
                        sprintf(message1, "Message from %d\n", getpid());
                        write(fd, message1, strlen(message1));
                        if ((i == orisma) | (i == orisma - 1))
                        {
                                printf("Child 1 terminated.\n");
                                exit(0);
                        }
                        sleep(1);
                }
        }
        else
        {
                pid_t child_2 = fork();
                if (child_2 < 0)
                {
                        perror("fork");
                }
                else if (child_2 == 0)
                {
                        printf("[Child 2] Started. PID=%d PPID=%d\n", getpid(), getppid());
                        sleep(1);
                        for (i = 1; i < orisma + 1; i += 2)
                        {
                                printf("[Child 2] Heartbeat. PID=%d Time=%ld x=%d\n", getpid(), time(NULL), i);
                                sprintf(message1, "Message from %d\n", getpid());
                                write(fd, message1, strlen(message1));
                                if ((i == orisma) | (i == orisma - 1))
                                {
                                        printf("Child 2 terminated.\n");
                                        exit(0);
                                }
                                sleep(1);
                        }
                        exit(0);
                }
                else
                {
                        for (i = 0; i < orisma + 1; i += 2)
                        {
                                printf("[Parent] Heartbeat PID=%d Time=%ld\n", getpid(), time(NULL));
                                sprintf(message1, "Message from %d\n", getpid());
                                write(fd, message1, strlen(message1));
                                if ((i == orisma) | (i == orisma - 1))
                                        printf("Parent waiting for childs.\n");
                                sleep(1);
                        }
                        int res;
                        res = wait(&status);
                        printf("Child with PID=%d terminated.\n", res);
                        printf("Parent waiting for the other child.\n");
                        res = wait(&status);
                        printf("Child with PID=%d terminated.\n", res);
                        printf("Parent printing file:\n");
                        close(fd);
                        fd = open(argv[1], O_RDONLY, PERMS);

                        n_read = read(fd, buffer, BUFFER_SIZE);
                        if (n_read == -1)
                        {
                                perror("read");
                                exit(-1);
                        }
                        n_write = write(FD_STDOUT, buffer, n_read);
                        if (n_write < n_read)
                        {
                                perror("write");
                                exit(-1);
                        }
                        close(fd);
                }
        }
}
