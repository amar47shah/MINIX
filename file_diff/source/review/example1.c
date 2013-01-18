/* example1.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// preprocessor definitions
//      PI is constant float
//      MAX is a function
//          input: A and B
//          return: true or false
#define PI 3.14F
#define MAX(A,B) ((A)>(B)?(A):(B))

// two parameters to main():
//      argv[] is an array of character memory addresses
//          each: the initial mem address of a char string
//          the char strings themselves are
//              the BASH command
//                  and all the arguments in the commandline
//      argc is the length of the array argv[]
int main(int argc, char *argv[]){
// declare variables at beginning
  FILE *fp;
  char *buffer, *tmp;
  
  // if there are fewer than 2 members of argv then exit
  //    (need command + at least one argument)
  if (argc < 2) exit(1); // exit as failure
  // otherwise, we can open the file represented by
  //    the first argument
  fp = fopen(argv[1],"r");
  // if the file could not be opened (fp is null) then exit
  if (!fp) exit(1);
  // dynamic allocation: room for 1023 chars + null
  //    starting address saved as the char pointer
  //        named 'buffer'
  buffer = (char *)malloc(1024*sizeof(char));
  // if mem alloc was unsuccessful then exit
  if (!buffer) exit(1);
  // repeat until break:
  while (1){
    // fill up the allocated memory begun at 'buffer'
    //      with input from the file
    // QUESTION: what is the value of tmp?
    // what does it represent?
    // what does fgets() return?
    // also how does fgets() work?
    // ANSWER: fgets reads at most 1023 characters from fp
    //      appends null char and saves at most 1024
    //          chars to 'buffer'
    //      fgets returns the address of buffer if succ
    //      else returns NULL
    tmp = fgets(buffer,1024,fp);
    // if there was nothing to fgets, then break loop
    if (!tmp) break;
    // print (as an integer) the length of the string
    //  that begins at the address 'buffer'
    //  won't this just be 1024
    //      until the last line?
    printf("%d\n",strlen(buffer));
  }
  // close the file and free the memory,
  fclose(fp);
  free(buffer);
  // if you get this far, then everything was ok.
  return 0;
}
