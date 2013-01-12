#include <stdio.h>
#include <stdlib.h>

/*env var as a parameter to main*/
int main(int argc, char *argv[], char *envp[]){
  int i;

  i = 0;
  while (envp[i] != NULL){
    printf("env var %d: (%s)\n",i,envp[i]);
    i++;
  }
  return 0;
}
