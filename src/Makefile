CC=clang
CFLAGS=-I. -arch arm64 -arch x86_64
LDFLAGS=-arch arm64 -arch x86_64

peek_tree: peek_tree.o
	$(CC) $(LDFLAGS) -o peek_tree peek_tree.o

peek_tree.o: peek_tree.c
	$(CC) $(CFLAGS) -c peek_tree.c

clean:
	rm -f *.o peek_tree