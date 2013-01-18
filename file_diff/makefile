objects = file_diff.o hash_table.o
cc = gcc

# Link command:
file_diff : $(objects)
	gcc $(objects) -o file_diff
    
# Compilation commands:
file_diff.o : file_diff.c hash_table.h
	gcc -c file_diff.c
    
hash_table.o : hash_table.c hash_table.h
	gcc -c hash_table.c
