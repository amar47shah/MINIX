#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define NumEnvVar 64
#define ScratchSize 1024

/*env var as a global variable*/
extern char *const *environ;

struct EnvVar{
  char *name, *value;
};

/*used by qsort*/
int cmpName(const void *a, const void *b){
  struct EnvVar *p, *q;
  p = (struct EnvVar *)a;
  q = (struct EnvVar *)b;
  return strcmp(p->name,q->name);
}

int main(int argc, char *argv[]){
  int i, numEnvVar;
  char *ptr, *scratch;
  struct EnvVar myEnvVar[NumEnvVar];

  scratch = (char*)malloc(sizeof(char)*ScratchSize);
  if (scratch == NULL){
    perror("scratch");
    exit(1);
  }
  i = 0;
  while (environ[i] != NULL){
    if (strlen(environ[i])+1 > ScratchSize){
      printf("env var too long\n");
      return 1;
    }
    strcpy(scratch,environ[i]);
    ptr = strtok(scratch,"=");
    /*notice: (strlen(ptr)+1)*/
    myEnvVar[i].name = (char*)malloc(sizeof(char)*(strlen(ptr)+1));
    if (myEnvVar[i].name == NULL){
      perror("myEnvVar[i].name");
      exit(1);
    }
    strcpy(myEnvVar[i].name,ptr);
    ptr = strtok(NULL," ");
    /*notice: (strlen(ptr)+1)*/
    myEnvVar[i].value = (char*)malloc(sizeof(char)*(strlen(ptr)+1));
    if (myEnvVar[i].value == NULL){
      perror("myEnvVar[i].value");
      exit(1);
    }
    strcpy(myEnvVar[i].value,ptr);
    i++;
  }
  numEnvVar = i;
  printf("%d env variables\n\n",numEnvVar);
  for(i=0;i<numEnvVar;i++){
    printf("%s\t%s\n",myEnvVar[i].name,myEnvVar[i].value);
  }
  printf("\n\nSorted by name\n");
  qsort((void *)myEnvVar,numEnvVar,sizeof(struct EnvVar),cmpName);
  for(i=0;i<numEnvVar;i++){
    printf("%s\t%s\n",myEnvVar[i].name,myEnvVar[i].value);
  }
  for(i=0;i<numEnvVar;i++){
    free(myEnvVar[i].name);
    free(myEnvVar[i].value);
  }
  free(scratch);
  return 0;
}
