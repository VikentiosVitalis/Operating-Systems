#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <sys/wait.h>

bool isInt (char str[]) {
  for (int i = 0; i < strlen(str); i++) {
    if (isdigit(str[i]) == false)
    return false;
  }
  return true;
}

int main (int argc,char *argv[]) {
  
  int num1,r,status,num,number,childs,f=1;

  if (argc!=3) {
    printf("Give executable and 2 arguments!\n");
    exit(1);
  }
  if ((!isInt(argv[1])) | (!isInt(argv[2]))) {
    printf("Both arguments must be integers\n");
    exit (2);
  }
  childs=atoi(argv[1]);
  number=atoi(argv[2]);
  if (((childs==0)|(childs==1)) | (number==0)) {
    printf("First argument greater than 1 and second argument greater than 0\n");
    exit (3);
  }
  int child[childs],pi[childs+1][2];
  for (int t=0; t<childs+1; t++) {
    if (pipe(pi[t])!=0) {
      perror("pipe");
      exit (4);
    }
  }
  for (int i=0; i<childs; i++) {
    child[i]=fork();
    if (child[i]<0) {
       perror("fork");
       exit(5);
    }
    else if (child[i]==0) {
   r=i+1;
   int val;
   while (r<=number) {
     if (i==0) {
       if (r==1) {
         close(pi[0][1]);
         read(pi[0][0],&val,sizeof(int));
         printf("PID of the child procedure is %d and the factorial caclulated is %d\n",getpid(),val);
         val=val*r;
         if (r==number) {
           printf("Factorial caclulated is: %d\n",val);
           break;
         }
         close(pi[1][0]);
         write(pi[1][1],&val,sizeof(int));
         r+=childs;
        }
       else  if (r!=1) {
         
         close(pi[childs][1]);
         read(pi[childs][0],&val,sizeof(int));
         printf("PID of the child procedure is %d and the factorial caclulated is %d\n",getpid(),val);
         val=val*r;
         if (r==number) {
           printf("Factorial caclulated is: %d\n",val);
           break;
         }
         close(pi[1][0]);
         write(pi[1][1],&val,sizeof(int));
         r+=childs;
       }
     }
     else if (i!=0) {
       close(pi[i][1]);
       read(pi[i][0],&val,sizeof(int));
       printf("PID of the child procedure is %d and the factorial caclulated is %d\n",getpid(),val);
       val=val*r;
       if (r==number) {
         printf("Factorial caclulated is: %d\n",val);
         break;
       }
       close(pi[i+1][0]);
       write(pi[i+1][1],&val,sizeof(int));
       r+=childs;
      }
   }
   exit(0);
    }
      }
      num=1;
      close(pi[0][0]);
      write(pi[0][1],&num,sizeof(num));
      for (int q=0; q<childs; q++) {
        waitpid(child[q],&status,0);
      }
}
