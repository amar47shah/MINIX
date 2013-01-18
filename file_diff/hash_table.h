/*
 * hash_table.h : API for an abstract hash table
 */

/*
 * By declaring hash_table_ptr to be a void* we make the type
 * opaque to the user - she doesn't know whats in the structure
 * and therefore won't be tempted to mess with the individual
 * members.
 */

typedef void *hash_table_ptr;

/*
 * initialize a hash table. Note that we pass in a pointer
 * to a hash function.
 * The eq function is called to test whether two key values are
 * equivalent. This function should return 1 if the two keys
 * are equal and 0 otherwise.
 */

hash_table_ptr init_hash_table(int (*hash_function)(void *key),
			       int (*eq)(void *key1, void *key2),
			       int number_of_hash_buckets);

/*
 * lookup an element in the hash table based on the key returns
 * the associated object if found else it returns null
 */
void *lookup(hash_table_ptr hash_table, void *key);

/*
 * add an element to the hash table. The function returns 0
 * if the addition was successful, 1 if the key was already
 * present in the hash table,  2 if the hash code was out of range
 * and 3 if there was a memory allocation error.
 */
int add(hash_table_ptr hash_table, void *key, void *obj);

void delete(hash_table_ptr hash_table, void *key);

/*
 * Apply a given function to all of the elements in the
 * table.
 */

void map(hash_table_ptr hash_table, void (*function)(void *key, void *obj));

/*
 * Convert the hash table into one large linked list and
 * print out the sorted list (using mergesort).
 */

void convert_sort_print(hash_table_ptr hash_table);
