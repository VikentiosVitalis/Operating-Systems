#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h>
#include <math.h>
#include <time.h>
#include <string.h>

int max (int a,int b) {
  if (a>b) return a;
  else return b;
}

int port,x,y,value=0;
double z;
long int w;
bool debug=false;
char *host,buffer[50];
char *strings[5];

int main (int argc,char* argv[]) {
  if (argc>6) {  //Elegxos orismatwn
    printf ("Input must include executable file and probably one of these:--host HOST --port PORT --debug\n");
    exit(1);
  }
  else if (argc==1) {
    host="lab4-server.dslab.os.grnetcloud.net";
    port=18080;
  }
  else if (argc==2) {
    if (strncmp(argv[1],"--debug",7)!=0) {
      printf("Second argument must be --debug\n");
      exit(2);
    }
    else {
      debug=true;
      host="lab4-server.dslab.os.grnetcloud.net";
      port=18080;
    }
  }
  else {
    for (int h=1; h<argc; h++) {
      if (strncmp(argv[h],"--debug",7)==0) {
        debug=true;
      }
      else if (strncmp(argv[h],"--host",6)==0) {
        if (h==argc-1) {
          printf("Last argument can't be --host");
          exit(3);
        }
        else if ((strncmp(argv[h+1],"lab4-server.dslab.os.grnetcloud.net",35)!=0) | (strlen(argv[h+1])!=35)) {
          printf("Can't connect to this host!\n");
          exit(4);
        }
        else {
          host="lab4-server.dslab.os.grnetcloud.net";
          port=18080;
        }
      }
      else if (strncmp(argv[h],"--port",6)==0) {
        if (h==argc-1) {
          printf("Last argument can't be --port");
          exit(5);
        }
        else if ((strncmp(argv[h+1],"18080",5)!=0) | (strlen(argv[h+1])!=5)) {
          printf("Can't connect to this port!\n");
          exit(6);
        }
        else {
          host="lab4-server.dslab.os.grnetcloud.net";
          port=18080;
        }
      }
    }
  }
  strings[0]="boot";  //Orismos pinakas gia thn get gia thn analogh timh
  strings[1]="setup";
  strings[2]="interval";
  strings[3]="button";
  strings[4]="motion";
  //Parametroi socket
  int domain=AF_INET; 
  int type=SOCK_STREAM;
  int sock_fd=socket(domain,type,0);

  if (sock_fd<0) {  //Error sthn dhmiourgia socket
    perror("socket");
    exit(7);
  }

  struct sockaddr_in sin; //Diey8hnsiodothsh
  sin.sin_family = AF_INET;  //sin family idiou typou me to domain
  sin.sin_port = htons(0); //0 : syndeetai se opoidhpote port, to opoio to dialegei to systhma
  // Kanonika to sin port: kathorizei to port syndeshs tu socket kai den syndeetai kanena allo socket
  sin.sin_addr.s_addr = htonl(INADDR_ANY); //INADDR_ANY : syndeetai me opoiondhpote allon

  if(bind(sock_fd, &sin, sizeof(sin))<0) {
    perror("bind");
    exit(8);
  }
  // Xarakthristika tu server
  struct sockaddr_in server;
  server.sin_family = AF_INET;
  server.sin_port = htons(port);
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  //Diey8ynsh host
  struct hostent *hostp;
  hostp = gethostbyname(host); //Thn antigrafume sth diey8ynsh tu server
  // gia na syndesume to socket sth diey8nhsh tu host
  bcopy(hostp->h_addr,&server.sin_addr,hostp->h_length);

  int connection=connect(sock_fd,&server,sizeof(server)); //syndeume
  if (connection<0) { //se periptwsh apotyxias -> Error
    perror("connect");
    exit(9);
  }

  while (1) {
    fd_set inset; //Dhmiurgia set apo FDs
    int maxfd;
    FD_ZERO(&inset); //Ka8arizei olus tus file descriptors pu ypxarxun sto set
    FD_SET(0,&inset); //Bazei file descriptor sto set
    FD_SET(sock_fd, &inset); //Prosthetume kai to socket
    maxfd = max(0,sock_fd) + 1;
    int selection = select(maxfd, &inset, NULL, NULL, NULL); //Kathe fora epilegetai o FD
    if (selection<= 0) {
      perror("select");
      continue;
    }
    if (FD_ISSET(0,&inset)) { //Elegxos oti o FD yparxei ontws mesta sto set wste na paei na divasei
      if (value==0) {
        int n=read(0,buffer,sizeof(buffer));
        if (n==-1) {
          perror("read");
          exit(10);
        }
        else if (n==5 && strncmp(buffer,"help",4)==0) {
          printf("You can write 'help' which prints this help message or\n");
          printf("write 'exit' which closes socket and communication with server or\n");
          printf("write 'get' which gives you server data or\n");
          printf("you can write a message 'number name surname reason' for permission to go out\n");
        }
        else if (n==5 && strncmp(buffer,"exit",4)==0) {
          close(sock_fd);
          shutdown(sock_fd,2); // kleinun read kai write
          printf("Socket closed!\n");
          exit(0);
        }
        else if (n==4 && strncmp(buffer,"get",3)==0) { // get -> dedomena server
          value=1;
          write(sock_fd,"get",sizeof("get"));
        }
        else if (n>5) {
          value=2;
          if (write(sock_fd,buffer,n)<0) {
            perror("write");
            exit(11);
          }
          if (debug==true) {
            printf("[DEBUG] sent '");
            for (int i=0; i<n-1; i++)
            printf("%c",buffer[i]);
            printf("'\n");
          }
        }
      }
      else if (value==3) {
        int d=read(0,buffer,sizeof(buffer));
        value=4;
        if (debug==true) {
          printf("[DEBUG] sent '");
          for (int g=0; g<d-1; g++) {
            printf("%c",buffer[g]);
          }
          printf("'\n");
        }
        if(write(sock_fd,buffer,d)==-1) {
          perror("write");
          exit(12);
        }
      }
    }
    if (FD_ISSET(sock_fd,&inset)) {
      if (value==1) {
        read(sock_fd,buffer,22);
        if (debug==true) {
          printf("[DEBUG] ");
          printf("sent 'get'\n");
          printf("[DEBUG] ");
          printf("read '");
          for (int k=0; k<21; k++) {
            printf("%c",buffer[k]);
          }
          printf("'\n");
        }
        char* first_num=strtok(buffer," "); //spane ta dedomena tu buffer
        char* second_num=strtok(NULL," ");
        char* third_num=strtok(NULL," ");
        char* fourth_num=strtok(NULL," ");
        x=atoi(first_num); // metatroph akeraiwn
        y=atoi(second_num);
        z=atoi(third_num);
        w=atoi(fourth_num);
        int year=1900+localtime(&w)->tm_year; //struct mesw tu opoiou briskume to xrono 
        int month=localtime(&w)->tm_mon;
        int day=localtime(&w)->tm_mday;
        int hour=localtime(&w)->tm_hour;
        int minutes=localtime(&w)->tm_min;
        int seconds=localtime(&w)->tm_sec;
        printf("Latest event:\n");
        printf("%s (%d)\n",strings[x],x);
        z=z/100;
        printf("Temperature is: %0.2f\n",z);
        printf("Light level is: %d\n",y);
        printf("Timestamp is: %d-%d-%d %d:%d:%d\n",year,month,day,hour,minutes,seconds);
        value=0;
      }
      else if (value==2) {
        int m=read(sock_fd,buffer,sizeof(buffer)); 
        value=3;
        if (m==-1) {
          perror("read");
          exit(13);
        }
        if (strncmp(buffer,"try again",9)==0) {
          for (int g=0; g<9; g++) {
            printf("%c",buffer[g]);
          }
          printf("Wrong message!\n");
          exit(14);
        }
        if (debug==true) {
          printf("[DEBUG] read '");
          for (int g=0; g<m-1; g++) {
            printf("%c",buffer[g]);
          }
          printf("'\n");
        }
        printf("Send verification code: '");
        for (int k=0; k<m-1; k++) {
          printf("%c",buffer[k]);
        }
        printf("'\n");
      }
      else if (value==4) {
        int h=read(sock_fd,buffer,sizeof(buffer)); //diabasma kwdikoy
        if (h==-1) {
          perror("read");
          exit(15);
        }
        if (debug==true) {
          printf("[DEBUG] read '");
          if (buffer[0]=='A') { //An o prwtos xarakthras einai A ektypwnei to ACK kai to mhnyma pou dwsame
            for (int a=0; a<h-2; a++) {
              printf("%c",buffer[a]);
            }
            printf("'\n");
          }
          else {
            for (int f=0; f<h-1; f++) {
              printf("%c",buffer[f]);
            }
            printf("'\n");
          }
        }
        if (buffer[0]=='A') { 
          printf("Response: '");
          for (int f=0; f<h-2; f++)
          printf("%c",buffer[f]);
          printf("'\n");
        }
        else {
          printf("Send message and password again, password is wrong!\n");
        }
        value=0;
      }
    }
  }
}
