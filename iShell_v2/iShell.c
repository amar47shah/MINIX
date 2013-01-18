#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#define lineLen 1024
#define numArg 24
#define numRec 10
#define numPath 32
#define maxEnvVar 64

struct EnvVar{
  char *name, *value;
};

char **parseCmd(char cmdLine[]){
  char c, **cmdArg, *ptr, longCmdLine[lineLen*2];
  int i, j, status;

  /* 'cushion' all |, <, > with spaces */
  c = cmdLine[i=j=0];
  while (c){
    if (strchr("|<>", c)) longCmdLine[j++] = ' ';
    longCmdLine[j++] = c;
    if (strchr("|<>", c)) longCmdLine[j++] = ' ';
    c = cmdLine[++i];
  }
  longCmdLine[j] = '\0';  

  /* (numArg+1) because the list must be terminated by a NULL ptr*/
  cmdArg = (char **)malloc(sizeof(char *) * (numArg+1));
  if (cmdArg == NULL){
    perror("parseCmd cmdArg");
    exit(1);
  }
  /* "<=" */
  for(i=0;i<=numArg;i++) cmdArg[i] = NULL;

  i = 0;
  ptr = strtok(longCmdLine," ");
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
  char cmdLine[lineLen], **cmdArg;
  char hist[numRec][lineLen]; 
  char *scratch, *ptr, path[numPath][lineLen];
  char *fileName, *fileName1, *fileName2;
  pid_t pid, pid1, pid2;
  int oldIn, newIn, oldOut, newOut;
  int status, status1, status2, i, j, debug, histLen, histStart, numEnvVar;
  int pipeFound, pipeArg, fd[2]; 
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
    if (cmdArg[0] == NULL){
      continue;
    }
    else if (strcmp(cmdArg[0],"exit") == 0){
      if (debug) printf("exiting\n");
      printf("Thank you for using iShell\n");
      break;
    }
    else if (strcmp(cmdArg[0],"env") == 0){
      if (debug) printf("listing environment variables\n");

      /* Output redirection */
      newOut = -1;
      if (cmdArg[1] != NULL && strcmp(cmdArg[1],">") == 0){
        if (cmdArg[2] != NULL){
          fflush(stdout);
          oldOut = dup(1);
          newOut = open(cmdArg[2], O_WRONLY | O_CREAT);
          if (newOut >= 0){
            dup2(newOut, 1);
            close(newOut);
          }
          else{
            printf("unable to open file for output\n");
          } 
        }
      }

      for(i=0;i<numEnvVar;++i)
        printf("env var %d:\t%s=%s\n",i,myEnvVar[i].name,myEnvVar[i].value);

      /* Reset stdout */
      if (newOut >= 0){
        dup2(oldOut, 1);
        close(oldOut);
      }
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

      /* Output redirection */
      newOut = -1;
      if (cmdArg[1] != NULL && strcmp(cmdArg[1],">") == 0){
        if (cmdArg[2] != NULL){
          fflush(stdout);
          oldOut = dup(1);
          newOut = open(cmdArg[2], O_WRONLY | O_CREAT);
          if (newOut >= 0){
            dup2(newOut, 1);
            close(newOut);
          }
          else{
            printf("unable to open file for output\n");     
          }
        }
      }

      for(i=0;i<histLen;++i) printf("%s\n", hist[(histStart+i)%10]);

      /* Reset stdout */
      if (newOut >= 0){
        dup2(oldOut, 1);
        close(oldOut);
      }
    }  
    else{
      /* MINIX commands */

      /* first make a list of paths */

      /* get PATH variable value */     
      i = 0;
      while (i < numEnvVar){
        if (strcmp(myEnvVar[i].name, "PATH") == 0){
          scratch = (char*)malloc(
                    sizeof(char)*(strlen(myEnvVar[i].value)+1));
          if (scratch == NULL){
            perror("scratch");
            exit(1);
          }
          strcpy(scratch,myEnvVar[i].value);
          break;
        }
        ++i;
      } 

      /* obtain and save individual paths */
      i = 0;
      ptr = strtok(scratch,":");
      while (ptr != NULL){
        strcpy(path[i], ptr);
        ++i;          
        ptr = strtok(NULL,":");
      }
      free(scratch);
      /* a list of paths is now saved in path[] */

      /* is there a pipe? */
      pipeFound = 0;
      i = 1;  /* cmdArg[0] cannot be '|' */
      while (cmdArg[i] != NULL){
        if (strcmp(cmdArg[i], "|") == 0){
          pipeFound = 1;
          pipeArg = i;
          break;
        }
        i++;
      }

      if (pipeFound){
        /* there is a pipe */
        if (debug) printf("Found pipe: (%d)\n", pipeArg);
        if (cmdArg[i+1] == NULL){
          /* there is nothing following the '|' */
          if (debug) printf("| without command\n");
          continue;
        } 
        cmdArg[pipeArg] = NULL;

        /* locate command files */
        fileName1 = (char*)malloc(sizeof(char)*lineLen);
        if (fileName1 == NULL){
          perror("fileName1");
          exit(1);
        }
        i = 0;
        while (i < numPath && path[i] != NULL){
          strcpy(fileName1, strcat(strcat(path[i],"/"),cmdArg[0]));
          if (access(fileName1, F_OK) == 0){
            /* found a good path */
            if (debug) printf("%s\n", fileName1);
            break;
          }
          i++;
        }
        if (i==numPath || path[i]==NULL){
          printf("%s not found\n",cmdArg[0]);  
          free(fileName1);
          continue;
        }

        fileName2 = (char*)malloc(sizeof(char)*lineLen);
        if (fileName2 == NULL){
          perror("fileName2");
          exit(1);
        }
        i = 0;
        while (i < numPath && path[i] != NULL){
          strcpy(fileName2, strcat(strcat(path[i],"/"),cmdArg[pipeArg+1]));
          if (access(fileName2, F_OK) == 0){
            /* found a good path */
            if (debug) printf("%s\n", fileName2);
            break;
          }
          i++;
        }
        if (i==numPath || path[i]==NULL){
          printf("%s not found\n",cmdArg[pipeArg+1]);  
          free(fileName1);
          free(fileName2);
          continue;
        }
       
        pipe(fd);
        pid2 = fork();
        if (pid2 == 0){ 
          /* I am the child for the 2nd part of pipe */
          
          /* redirect I/O for pipe */
          close(fd[1]); /* close pipe output */
          close(0);     /* close stdin */
          dup(fd[0]);   /* dup pipe input to stdin */
          close(fd[0]); /* close pipe input */
        
          /* check for output redirection */
          i = pipeArg + 2;
          while (cmdArg[i] != NULL){
            if (strcmp(cmdArg[i], ">") == 0){
              /* redirect output */
              if (debug) printf("attempting to redirect output\n");
              if (cmdArg[i+1] != NULL){
                newOut = open(cmdArg[i+1], O_WRONLY | O_CREAT);
                if (newOut >= 0){
                  close(1);
                  dup(newOut);
                  close(newOut);
                }
                else{
                  if (debug) printf("couldn't open output file\n");
                }
              }
              else {
                if (debug) printf ("> without filename\n");
                break;
              }
              cmdArg[i] = NULL;
            }
            i++;
          }
          
          /* ready to execute 2nd part of pipe */
          status2 = execv(fileName2, cmdArg + pipeArg+1);        
        } 
        else{
          pid1 = fork();
          if (pid1 == 0){
            /* I am the child for the 1st part of the pipe */

            /* redirect I/O for pipe */
            close(fd[0]); /* close pipe input */
            close(1);     /* close stdout */
            dup(fd[1]);   /* dup pipe output to stdout */
            close(fd[1]); /* close pipe output */ 

            /* check for input redirection */
            i = 1;
            while (cmdArg[i] != NULL){
              if (strcmp(cmdArg[i], "<") == 0){
                /* redirect input */
                if (debug) printf("attempting to redirect input\n");
                if (cmdArg[i+1] != NULL){
                  newIn = open(cmdArg[i+1], O_RDONLY | O_CREAT);
                  if (newIn >= 0){
                    close(0);
                    dup(newIn);
                    close(newIn);
                  }
                  else{
                    if (debug) printf("couldn't open input file\n");
                  }
                }
                else {
                  if (debug) printf ("< without filename\n");
                  break;
                }
                cmdArg[i] = NULL;
              }
              i++;
            }
  
            /* ready to execute 1st part of pipe */
            status1 = execv(fileName1, cmdArg); 
          }
          else{
            /* I am the parent process */
            close(fd[0]);
            close(fd[1]);
            waitpid(pid1,&status1,0);
          }
          waitpid(pid2,&status2,0);        
        }
  
        /* free memory */
        free(fileName1);
        free(fileName2);
  
      } 
      else{
        /* no pipe */

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
              /* I am the child process */
              
              /* I/O Redirection */
              j = 1;
              while (cmdArg[j] != NULL){
                if (strcmp(cmdArg[j], "<") == 0){
                  /* redirect input */
                  if (debug) printf ("attempting to redirect input\n");
                  if (cmdArg[j+1]  != NULL){
                    newIn = open(cmdArg[j+1], O_RDONLY | O_CREAT);
                    if (newIn >= 0){
                      close(0);
                      dup(newIn);
                      close(newIn);
                    }
                    else{
                      if (debug) printf ("couldn't open input file\n");
                    }
                  }
                  else{
                    /* bad syntax, abort I/O redirection */
                    if (debug) printf ("< without filename\n");
                    break;
                  }
                  cmdArg[j] = NULL;
                }
                else if (strcmp(cmdArg[j], ">") == 0){
                  /* redirect output */
                  if (debug) printf("attempting to redirect output\n"); 
                  if (cmdArg[j+1] != NULL){
                    newOut = open(cmdArg[j+1], O_WRONLY | O_CREAT);
                    if (newOut >= 0){
                      close(1);
                      dup(newOut);
                      close(newOut);  
                    }
                    else{
                      if (debug) printf ("couldn't open output file\n");
                    } 
                  }
                  else{
                    /* bad syntax, abort I/O redirection */
                    if (debug) printf ("> without filename\n");
                    break;
                  }
                  cmdArg[j] = NULL;
                }
                j++;
              }
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

