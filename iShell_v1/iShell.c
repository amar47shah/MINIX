#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define lineLen 1024
#define numArg 8
#define numRec 10
#define numPath 32
#define maxEnvVar 64

struct EnvVar{
  char *name, *value;
};

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

int main(int argc, char *argv[], char *envp[]){
  char cmdLine[lineLen], **cmdArg, hist[numRec][lineLen],
       *scratch, *ptr, path[numPath][lineLen], *fileName;
  pid_t pid;
  int status, i, debug, histLen, histStart, numEnvVar;
  struct EnvVar myEnvVar[maxEnvVar];

  /* set history queue */
  histLen = 0;
  histStart = 0;

  /* create array of structs myEnvVar */
  scratch = (char*)malloc(sizeof(char)*lineLen);
  if (scratch == NULL){
    perror("scratch");
    exit(1);
  }
  i = 0;
  while (envp[i] != NULL){
    if (strlen(envp[i])+1 > lineLen){
      printf("env var too long\n");
      return 1;
    }
    strcpy(scratch,envp[i]);
    ptr = strtok(scratch,"=");
    myEnvVar[i].name = (char*)malloc(sizeof(char)*(strlen(ptr)+1));
    if (myEnvVar[i].name == NULL){
      perror("myEnvVar[i].name");
      exit(1);
    }
    strcpy(myEnvVar[i].name,ptr);
    ptr = strtok(NULL," ");
    myEnvVar[i].value = (char*)malloc(sizeof(char)*(strlen(ptr)+1));
    if (myEnvVar[i].value == NULL){
      perror("myEnvVar[i].value");
      exit(1);
    }
    strcpy(myEnvVar[i].value,ptr);
    ++i;
  }
  numEnvVar = i;
  free(scratch);

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
    if (strcmp(cmdLine,"") == 0) continue;

    if (histLen<10)
      ++histLen;
    else
      histStart = ++histStart%10;
    strcpy(hist[(histStart+histLen-1)%10],cmdLine);

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
    else if (strcmp(cmdArg[0],"env") == 0){
      if (debug) printf("listing environment variables\n");
      for(i=0;i<numEnvVar;++i)
        printf("env var %d:\t%s=%s\n",i,myEnvVar[i].name,myEnvVar[i].value);
    }
    else if (strcmp(cmdArg[0],"setenv") == 0){
      if (debug) printf("setting environment variable\n");
      if (cmdArg[1] == NULL || cmdArg[2] == NULL){
        printf("syntax: setenv variable value\n");
        continue;
      }
      i = 0;
      while (i < numEnvVar){
        if (strcmp(myEnvVar[i].name,cmdArg[1]) == 0){
          myEnvVar[i].value = (char*)realloc(myEnvVar[i].value,
                                             sizeof(char)*
                                             (strlen(cmdArg[2])+1));
          if (myEnvVar[i].value == NULL){
            perror("old myEnvVar[i].value");
            exit(1);
          }
          strcpy(myEnvVar[i].value,cmdArg[2]);
          break;
        }
        ++i;
      }
      /* make new environment variable */
      if (i == numEnvVar){
        myEnvVar[i].name = (char*)malloc(sizeof(char)*(strlen(cmdArg[1])+1));
        if (myEnvVar[i].name == NULL){
          perror("new myEnvVar[i].name");
          exit(1);
        }
        strcpy(myEnvVar[i].name,cmdArg[1]);
        myEnvVar[i].value = (char*)malloc(sizeof(char)*(strlen(cmdArg[2])+1));
        if (myEnvVar[i].value == NULL){
          perror("new myEnvVar[i].value");
          exit(1);
        }
        strcpy(myEnvVar[i].value,cmdArg[2]);
        numEnvVar++;
      }
    }
    else if (strcmp(cmdArg[0],"unsetenv") == 0){
      if (debug) printf("removing environment variable\n");
      if (cmdArg[1] == NULL){
        printf("syntax: unsetenv variable\n");
        continue;
      }
      i = 0;
      while (i < numEnvVar){
        if (strcmp(myEnvVar[i].name,cmdArg[1]) == 0) break;
        ++i;
      }
      if (i == numEnvVar){
        printf("env var not found\n");
      }
      else{
        while (i < numEnvVar-1){
          myEnvVar[i].name = (char*)realloc(myEnvVar[i].name,
                                            sizeof(char)*
                                            (strlen(myEnvVar[i+1].name)+1));
          if (myEnvVar[i].name == NULL){
            perror("myEnvVar[i].name");
            exit(1);
          }
          strcpy(myEnvVar[i].name,myEnvVar[i+1].name);
          myEnvVar[i].value = (char*)realloc(myEnvVar[i].value,
                                             sizeof(char)*
                                             (strlen(myEnvVar[i+1].value)+1));
          if (myEnvVar[i].value == NULL){
            perror("myEnvVar[i].value");
            exit(1);
          }
          strcpy(myEnvVar[i].value,myEnvVar[i+1].value);
          ++i;
        }
        free(myEnvVar[i].name);
        free(myEnvVar[i].value);
        numEnvVar--;
      }
    }
    else if (strcmp(cmdArg[0],"cd") == 0){
      if (debug) printf("changing working directory\n");
      if (chdir(cmdArg[1] ? cmdArg[1] : "/root") == 0){
        /* update CWD */
        i = 0;
        while (i < numEnvVar){
          if (strcmp(myEnvVar[i].name, "PWD") == 0){
            myEnvVar[i].value = (char*)realloc(myEnvVar[i].value,
                                               sizeof(char)*lineLen);
            if (myEnvVar[i].value == NULL){
              perror("myEnvVar[i].value");
              exit(1);
            }
            getcwd(myEnvVar[i].value, lineLen-1);
            break;
          }
          ++i;
        }
      }
    }
    else if (strcmp(cmdArg[0],"history") == 0){
      if (debug) printf("displaying the last ten commands\n");
      for(i=0;i<histLen;++i) printf("%s\n", hist[(histStart+i)%10]);
    }
    else{
      /* get PATH */
      i = 0;
      while (i < numEnvVar){
        if (strcmp(myEnvVar[i].name, "PATH") == 0){
          scratch = (char*)malloc(sizeof(char)*(strlen(myEnvVar[i].value)+1));
          if (scratch == NULL){
            perror("scratch");
            exit(1);
          }
          strcpy(scratch,myEnvVar[i].value);
          break;
        }
        ++i;
      }
      /* break into tokens and save them */
      i = 0;
      ptr = strtok(scratch,":");
      while (ptr != NULL){
        strcpy(path[i], ptr);
        ++i;
        ptr = strtok(NULL,":");
      }
      free(scratch);
      /* check for file */
      fileName = (char*)malloc(sizeof(char)*lineLen);
      if (fileName == NULL){
        perror("fileName");
        exit(1);
      }
      i = 0;
      while (i < numPath && path[i] != NULL){
        strcpy(fileName, strcat(strcat(path[i],"/"),cmdArg[0]));
        if (access(fileName, F_OK) == 0){
          if (debug) printf("going to fork\n");
          pid = fork();
          if (pid != 0){
            waitpid(pid,&status,0);
          }
          else{
            status = execv(fileName,cmdArg);
          }
          break;
        }
        i++;
      }
      if (i == numPath || path[i] == NULL) printf("command not found\n");
      /* free memory */
      free(fileName);
    }
    i = 0;
    while (cmdArg[i] != NULL) free(cmdArg[i++]);
    free(cmdArg);
  }
  for(i=0;i<numEnvVar;++i){
    free(myEnvVar[i].name);
    free(myEnvVar[i].value);
  }
  return 0;
}

