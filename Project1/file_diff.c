#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hash_table.h"

#define NUM_BUCKETS 59

static int hash_function(void *key){
  char *str = (char *)key;
  int sum = 0;
  while (*str != '\0') sum += *(str++);
  return (sum%NUM_BUCKETS);
}

static int eq(void *key1, void *key2){
  if (!key1 || !key2)
    return 0;
  return (!strcmp((char *)key1,(char *)key2));
}

char *normalize (char *str) {
  int i, j;
  i = j = 0;
  while (str[i]!= '\0'){
    if (isalpha(str[i])) str[j++] = toupper(str[i]);
	i++;
  }
  str[j] = '\0';
  return str;
}

void process_file (hash_table_ptr hash_table, char* filename, int is_source_file){
  FILE *fp;
  char buffer[1024], copy[1024], *tmp_read, *tmp_tok, *token;

  fp = fopen(filename,"r");
  if (!fp) exit(1);

  while (1){
    tmp_read = fgets(buffer, 1024, fp);
    if (!tmp_read) break;

    buffer[strlen(buffer)-1] = '\0';
    strcpy(copy, buffer);
    tmp_tok = strtok(copy, " ");

    while (tmp_tok){
      if (is_source_file){
        token = (char *)malloc(sizeof(char) * (strlen(tmp_tok) + 1));
        strcpy(token, tmp_tok);
        add(hash_table, (void *)normalize(token), NULL);
      }
      else
        delete(hash_table, (void *)normalize(tmp_tok));
      tmp_tok = strtok(NULL, " ");
    }
  }
  fclose(fp);

}


int main(int argc, char *argv[]){
  hash_table_ptr hash_table;

  if (argc < 3) exit(1);

  hash_table = init_hash_table (hash_function, eq, NUM_BUCKETS);
  if (!hash_table){
    printf ("Error initializing hash table\n");
    exit (1);
  }

  process_file(hash_table, argv[1], 1);
  process_file(hash_table, argv[2], 0);

  convert_sort_print(hash_table);

  return 0;
}

