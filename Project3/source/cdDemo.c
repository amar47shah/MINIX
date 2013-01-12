#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define lineLen 1024
#define numArg 8

char **parseCmd(char cmdLine[]){
  char **cmdArg, *ptr;
  int i, status;

  /* (numArg+1) because the list must be terminated by a NULL ptr*/
  cmdArg = (char **)malloc(sizeof(char *) * (numArg+1));
  if (cmdArg == NULL){
    perror("parseCmd cmdArg");
    exit(1);
  }
  /* "<=" */
  for(i=0;i<=numArg;i++) cmdArg[i] = NULL;
  i = 0;
  ptr = strtok(cmdLine," ");
  while (ptr != NULL){
    /* (strlen(ptr)+1) */
    cmdArg[i] = (char *)malloc(sizeof(char)*(strlen(ptr)+1));
    if (cmdArg[i] == NULL){
      perror("parseCmd cmdArg[i]");
      exit(1);
    }
    strcpy(cmdArg[i++],ptr);
    if (i == numArg) break;
    ptr = strtok(NULL," ");
  }
  return(cmdArg);
}

int main(int argc, char *argv[]){
  char cmdLine[lineLen], **cmdArg;
  pid_t pid;
  int status, i, debug;

  debug = 0;
  i = 1;
  while (i<argc){
    if (!strcmp(argv[i],"-d")) debug = 1;
    i++;
  }
  while (1){
    printf("iShell> ");
    fgets(cmdLine,lineLen,stdin);
    cmdLine[strlen(cmdLine)-1] = '\0';
    cmdArg = parseCmd(cmdLine);
    if (debug){
      i = 0;
      while (cmdArg[i] != NULL){
	printf("\t%d (%s)\n",i,cmdArg[i]);
	i++;
      }
    }
    if (strcmp(cmdArg[0],"exit") == 0){
      if (debug) printf("exiting\n");
      printf("Thank you for using iShell\n");
      break;
    }
    else if (strcmp(cmdArg[0],"cd") == 0){
      if (debug) printf("doing cd\n");
      chdir(cmdArg[1] ? cmdArg[1] : "/root");
    }
    else{
      if (debug) printf("going to fork\n");
      pid = fork();
      if (pid != 0){
	waitpid(pid,&status,0);
      }
      else{
	status = execv(cmdArg[0],cmdArg);
	if (status){
	  printf("\tno such cmd (%s)\n",cmdArg[0]);
	  return 1;
	}
      }
    }
    i = 0;
    while (cmdArg[i] != NULL) free(cmdArg[i++]);
    free(cmdArg);
  }
  return 0;
}
