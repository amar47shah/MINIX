#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#define LineLen 1024

int main(int argc, char *argv[]){
  pid_t pid;
  int status;
  char line[LineLen];

  while (1){
    pid = fork();
    if (pid < 0){
      perror("fork failed\n");
      exit(1);
    }
    if (pid != 0){
      /*this is the parent process*/
      printf("Parent waiting for child %d\n",pid);
      waitpid(pid,&status,0);
      printf("Child status: %d\n",status);
      if (!status) break;
    }
    else{
      /*this is the child process*/
      printf("Child proc id %d, parent proc id %d\n",
	     getpid(),getppid());
      fgets(line,LineLen,stdin);
      line[strlen(line)-1] = '\0';
      if (strcmp(line,"quit") == 0) return 0;
      else return 1;
    }
  }
  return 0;
}
