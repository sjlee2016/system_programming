CC = gcc
OBJ = main.o
HEADER = 20161619.h 
CFLAGS = -c -Wall  -Iinclude

hello: $(OBJ) 
    $(CC) $(OBJ) -o $@

main.out: 20161619.c $(HEADER)
    $(CC) $(CFLAGS) $< -o $@

clean: 
    rm -rf *o hello