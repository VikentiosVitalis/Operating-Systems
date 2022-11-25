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
char *str,arg1[10],arg2[10];
int N, ret, status, *child, x;
int active;
int co;

void handleruser1father(int sig)
{
  for (int t = 0; t < N; t++)
  {
    kill(child[t], SIGUSR1);
  }
}

void handlertermfather(int sig)
{
  for (int q = 0; q < N; q++)
  {
    kill(child[q], SIGTERM);
    sprintf(buffer, "Child %d with PID=%d terminated successfully with exit status code 0!\n", q,child[q]);
    write(1, buffer, strlen(buffer));
    active--;
  }
}

void handlersigchfather(int sig)
{
  if (active==N) {
    ret = waitpid(-1, &status, 0);
    if (WIFSTOPPED(status))
    {
      kill(ret, SIGCONT);
    }
    else
    {
      co = 0;
      while (child[co] != ret)
      {
        co++;
      }
      child[co] = fork();
      if (child[co] < 0)
      {
        perror("fork");
      }
      else if (child[co] == 0)
      {
        sprintf(arg1, "%d", co);
        sprintf(arg2, "%d", getpid());
        char *const argv[]= {"./child", arg1, arg2, &str[co], NULL};
        execv(argv[0], argv);
      }
      else if (child[co] > 0)

      {
        sprintf(buffer, "[PARENT/PID=%d] Creatd new child for gate %d (PID %d) and initial state '%c'\n",getpid(),co,child[co],str[co]);
        write(1, buffer, strlen(buffer));
      }
    }
  }
}

int main(int argc, char *argv[])
{
  x = time(NULL);
  if (argc != 2)
  {
    printf("Give arguments again\n");
    exit(EXIT_FAILURE);
  }
  N = strlen(argv[1]);
  child = malloc(N * sizeof(*child));
  str = malloc(N * sizeof(*str));
  strcpy(str, argv[1]);
  for (int l = 0; l < N; l++)
  {
    if (str[l] != 't' && str[l] != 'f')
    {
      printf("Give string which contains characters 't' and 'f' only\n");
      exit(EXIT_FAILURE);
    }
  }

  for (int u = 0; u < N; u++)
  {
    child[u] = fork();
    if (child[u] < 0)
    {
      perror("fork");
    }
    else if (child[u] == 0)
    {
      sprintf(arg1, "%d", u);
      sprintf(arg2, "%d", getpid());
      char *const argv[]= {"./child", arg1, arg2, &str[u], NULL};
      execv(argv[0], argv);
    }
    else if (child[u] > 0)
    {
      active++;
      printf("[PARENT/PID=%d] Created Child %d (PID=%d) with initial state '%c'\n", getpid(), u, child[u], str[u]);
    }
  }

  struct sigaction actionuser1father;
  actionuser1father.sa_handler = handleruser1father;
  actionuser1father.sa_flags = SA_RESTART;
  sigaction(SIGUSR1, &actionuser1father, NULL);

  struct sigaction actiontermfather;
  actiontermfather.sa_handler = handlertermfather;
  actiontermfather.sa_flags = SA_RESTART;
  sigaction(SIGTERM, &actiontermfather, NULL);

  struct sigaction sigchldfather;
  sigchldfather.sa_handler = handlersigchfather;
  sigchldfather.sa_flags = SA_RESTART;
  sigaction(SIGCHLD, &sigchldfather, NULL);
  while (1)
  {
    if (active == 0)
    {
      printf("[Parent/PID=%d] All children exited terminating as well!\n",getpid());
      exit(0);
    }
  }
}
