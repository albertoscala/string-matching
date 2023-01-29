CC = clang
CFLAGS = -Wall -g

all: serial parallel

serial: aho-corasick-serial.c trie.c queue.c linkedlist.c hashtable.c aho-corasick.c
	$(CC) $(CFLAGS) aho-corasick-serial.c trie.c queue.c linkedlist.c hashtable.c aho-corasick.c -o serial

parallel:
	$(CC) $(CFLAGS)