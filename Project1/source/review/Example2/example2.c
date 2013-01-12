/* example2.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]){
  // buffer and copy can contain strings of max length 1023
  //    plus terminating null character
  // tmp is a char pointer
  // tokens is an array of (max) 127 char pointers
  //    each member of tokens can point to the beginning
  //    of a string
  char buffer[1024], copy[1024], *tmp, *tokens[128];
  int cnt = 0, i;

  printf("enter some words: ");
  // fgets from stdin a max of 1023 and inserts them
  //    in buffer with terminating null
  // if not successful, tmp is null
  tmp = fgets(buffer,1024,stdin);
  if (!tmp) exit(1);
  // the final non-NULL character of the string is
  //    overwritten as null. Why? Because it's
  //    a newline character?
  buffer[strlen(buffer)-1] = '\0';
  strcpy(copy,buffer);
  // on the first call, strtok() is sent the pointer
  //    copy, strtok() looks for the first " " and
  //    finds the word (from the beginning to the " ")
  //    returns the pointer to the beginning of the word
  tmp = strtok(copy," ");
  while (tmp != NULL){
    // allocate memory for a char array whose address
    //  is stored at tokens[cnt]
    tokens[cnt] = (char *)malloc(sizeof(char)*(strlen(tmp)+1));
    if (!tokens[cnt]) exit(1); //exit if alloc unsucc
    // copy the word (in tmp) into the list of words, inc cnt
    strcpy(tokens[cnt++],tmp);
    if (cnt == 128) break;
    // Clear tmp? Why? why like this?
    tmp = strtok(NULL," ");
  }
  // print out the words and free the memory. 
  for(i=0;i<cnt;i++){
    printf("%s\n",tokens[i]);
    free(tokens[i]);
  }
  return 0;
}
