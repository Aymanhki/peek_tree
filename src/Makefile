CC=clang
CFLAGS=-I.

# Detect OS
UNAME_S := $(shell uname -s)

# macOS specific flags
ifeq ($(UNAME_S),Darwin)
    CFLAGS += -arch arm64 -arch x86_64
    LDFLAGS += -arch arm64 -arch x86_64
endif

# Linux specific flags
ifeq ($(UNAME_S),Linux)
    CFLAGS += -march=native
endif

peek_tree: peek_tree.o
	$(CC) $(LDFLAGS) -o peek_tree peek_tree.o

peek_tree.o: peek_tree.c
	$(CC) $(CFLAGS) -c peek_tree.c

clean:
	rm -f *.o peek_tree