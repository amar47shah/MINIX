/* hash_table.c */

#include <stdlib.h>
#include <stdio.h>
#include "hash_table.h"

typedef struct {
  void *key;
  void *obj;
  void *next;
} hash_list_elt;

typedef struct {
  int (*hash_function)(void *key);
  int (*eq)(void *key1, void *key2);
  int number_of_hash_buckets;
  /* table points to an array of pointers to lists*/
  hash_list_elt **table;
} hash_table_private;

/*
 * initialize a hash table. Note that we pass in a pointer
 * to a hash function.
 */

hash_table_ptr init_hash_table(int (*hash_function)(void *key),
			       int (*eq)(void *key1, void *key2),
			       int number_of_hash_buckets){
  hash_table_private *hash_ptr;
  int i;

  /* Allocate the hash_table*/
  hash_ptr = malloc(sizeof(*hash_ptr));
  if (hash_ptr == NULL) return NULL;

  hash_ptr->hash_function = hash_function;
  hash_ptr->eq = eq;
  hash_ptr->number_of_hash_buckets = number_of_hash_buckets;

  /* Allocate space for the hash_buckets*/
  hash_ptr->table = malloc(number_of_hash_buckets * sizeof(hash_list_elt*));
  if (hash_ptr->table == NULL){
    free (hash_ptr);
    return NULL;
  } else {
    /* Initialize all the hash bucket lists to be empty*/
    for (i=0; i < number_of_hash_buckets; ++i)
      (hash_ptr->table)[i] = NULL;
  }

  return hash_ptr;
}

void *lookup(hash_table_ptr hash_table, void *key){
  hash_table_private *hash_ptr = hash_table;
  int hash_code;
  hash_list_elt *elt;

  /* Check that both hash_ptr and key are not NULL*/
  if (hash_ptr && key){
    hash_code = (hash_ptr->hash_function)(key);

    /* Check that the hash_code is in bounds*/
    if (hash_code < 0 || hash_code >= hash_ptr->number_of_hash_buckets)
      return NULL;
    else{
      elt = (hash_ptr->table)[hash_code];
      while (elt){
	/* Use the eq function to test whether you have found the key*/
	if ((hash_ptr->eq)(elt->key, key))
	  return elt->obj; /*If you find the key return the associated object*/
	elt = elt->next;
      }
    }
  }
  /* Default return value*/
  return NULL;
}

int add(hash_table_ptr hash_table, void *key, void *obj){
  hash_table_private *hash_ptr = hash_table;
  int hash_code;
  hash_list_elt *elt;

  /* Check that both hash_ptr and key are not NULL*/
  if (hash_ptr && key){
    /* Check if key is already in the table*/
    if (lookup (hash_table, key)) return 1;

    hash_code = (hash_ptr->hash_function)(key);
    /* Check that the hash_code is in bounds*/
    if (hash_code < 0 || hash_code >= hash_ptr->number_of_hash_buckets)
      return 2;

    elt = malloc(sizeof(*elt));
    if (elt == NULL) return 3;

    /* Push the element on the list*/
    elt->key = key;
    elt->obj = obj;
    elt->next = (hash_ptr->table)[hash_code];
    (hash_ptr->table)[hash_code] = elt;
    return 0;
  }
  return -1;
}

void delete(hash_table_ptr hash_table, void *key){
	hash_table_private *hash_ptr = hash_table;
	int hash_code;
	hash_list_elt *elt, *prev_elt;

	if (!hash_ptr || !key)
		return;

	hash_code = (hash_ptr->hash_function)(key);

	/* check that hash_code is in bounds*/
	if (hash_code < 0 || hash_code > hash_ptr->number_of_hash_buckets)
		return;

	/* find all instances of the key, cut them out, free the memory*/
	prev_elt = NULL;
	elt = (hash_ptr->table)[hash_code];
	while (elt) {
      if ((hash_ptr->eq)(elt->key, key)) {
		if (prev_elt)
		  prev_elt->next = elt->next;
        else
		  (hash_ptr->table)[hash_code] = elt->next;
		free(elt->key);
		free(elt); }
      else prev_elt = elt;
	  elt = elt->next;
	} /*end while*/
}


void map(hash_table_ptr hash_table,
	  void (*function)(void *key, void *obj)){
  hash_table_private *hash_ptr = hash_table;
  int hash_code;
  hash_list_elt *elt;

  if (hash_ptr && function){
    for(hash_code=0;hash_code<(hash_ptr->number_of_hash_buckets);++hash_code){
      /*remove this*/printf("\n%d ", hash_code);
      elt = (hash_ptr->table)[hash_code];
      while (elt){
        function(elt->key, elt->obj);
        elt = elt->next;
      }
    }
  }
}

void convert_sort_print(hash_table_ptr hash_table){
  hash_table_private *hash_ptr=hash_table;
  int hash_code=0, sorted=0, insize=1, psize, qsize, merge_count;
  hash_list_elt *head, *tail, *p, *q, *elt;

  /*convert hash table to one linked list*/
  while (!(hash_ptr->table)[hash_code] &&
		 hash_code<hash_ptr->number_of_hash_buckets)
    ++hash_code;
  if (hash_code>=hash_ptr->number_of_hash_buckets)
    head=tail=NULL;
  else{
    head=tail=(hash_ptr->table)[hash_code];
  	while (tail->next) tail=tail->next;
  	while (hash_code<hash_ptr->number_of_hash_buckets-1){
      tail->next=(hash_ptr->table)[++hash_code];
  	  while (tail->next) tail=tail->next;
  	}
  }

  /*sort list with merge sort*/
  if (head){
    while (!sorted){
      p=head;
      head=tail=NULL;
      merge_count=0;
      while (p){
        merge_count++;
        q=p;
        psize=0;
        while (q && psize<insize){
          psize++;
          q=q->next;
        }
        qsize=insize;
        while ((q && qsize>0) || psize>0){
          if (psize==0){
        	elt=q; q=q->next; qsize--; }
          else if (!q || qsize==0){
        	elt=p; p=p->next; psize--; }
          else if (strcmp((char *)p->key, (char *)q->key) <= 0){
            elt=p; p=p->next; psize--; }
          else {
        	elt=q; q=q->next; qsize--; }
          if (tail)
            tail->next=elt;
          else
        	head=elt;
          tail=elt;
        }
        p=q;
      }
      tail->next=NULL;
      sorted=(merge_count<=1);
      insize *=2;
    }
  }

  /*print list*/
  elt=head;
  while (elt){
    printf("%s\n", (char *)elt->key);
    elt=elt->next;
  }
}

