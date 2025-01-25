CC=clang
CFLAGS=-I.

peek_tree: peek_tree.o
	$(CC) -o peek_tree peek_tree.o

peek_tree.o: peek_tree.c
	$(CC) -c peek_tree.c $(CFLAGS)

clean:
	rm -f *.o peek_tree
