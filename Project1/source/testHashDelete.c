#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "hash_table.h"

#define NUM_BUCKETS 59
#define Size 200
#define Length 8
#define BigNum 999

extern long random(void);
extern void srandom(unsigned);

static int hash_function(void *key){
  char *str;
  int sum = 0;

  str = (char *)key;
  while (*str != '\0') sum += *(str++);
  return (sum%NUM_BUCKETS);
}

static int eq(void *key1, void *key2){
  return (!strcmp((char *)key1,(char *)key2));
}

static void generateStr(char *str, int length){
  int random_value;
  int i;

  for(i=0;i<length-1;i++){
    random_value = random();
    str[i] = (char)(random_value%26) + 'A';
  }
  str[length-1] = '\0';
}

static void testHash(hash_table_ptr hash_table){
  char S1[Size][Length], S2[Size][Length], *name;
  int num1 = 0, num2 = 0, i, j;

  for(i=0;i<BigNum;i++){
    name = (char *)malloc(sizeof(char)*Length);
    if (!name){
      printf("malloc failed\n");
      exit(1);
    }
    generateStr(name,Length);
    if (random() > 0X30000000){
      if (num1 == Size){
	for(j=0;j<Size;j++) delete(hash_table,(void *)(S1[j]));
	num1 = 0;
      }
      add(hash_table,(void *)name,NULL);
      strcpy(S1[num1++],name);
    } else {
      if (num2 == Size){
	for(j=0;j<Size;j++) delete(hash_table,(void *)(S2[j]));
	num2 = 0;
      }
      add(hash_table,(void *)name,NULL);
      strcpy(S2[num2++],name);
    }
  }
  for(i=0;i<num1;i++)
    printf("%s\n",S1[i]);
}

int main(void){
  hash_table_ptr hash_table;
  time_t seed;

  time(&seed);
  srandom(seed);
  hash_table = init_hash_table (hash_function, eq, NUM_BUCKETS);
  if (hash_table == NULL) {
    printf ("Error initializing hash table\n");
    exit (1);
  }
  testHash(hash_table);
  return 0;
}
