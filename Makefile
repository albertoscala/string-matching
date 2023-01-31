CC = clang
CFLAGS = -Wall -g

all: serial data-parallel

serial: aho-corasick-serial.c trie.c queue.c linkedlist.c hashtable.c aho-corasick.c
	$(CC) $(CFLAGS) aho-corasick-serial.c trie.c queue.c linkedlist.c hashtable.c aho-corasick.c -o serial

data-parallel: aho-corasick-data-parallel.c trie.c queue.c linkedlist.c hashtable.c aho-corasick.c
	$(CC) $(CFLAGS) aho-corasick-data-parallel.c trie.c queue.c linkedlist.c hashtable.c aho-corasick.c -o data-parallel -lpthread
