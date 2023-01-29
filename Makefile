CC = clang
CFLAGS = -Wall -g

all: serial task-parallel data-parallel

serial: aho-corasick-serial.c trie.c queue.c linkedlist.c hashtable.c aho-corasick.c
	$(CC) $(CFLAGS) aho-corasick-serial.c trie.c queue.c linkedlist.c hashtable.c aho-corasick.c -o serial

task-parallel: aho-corasick-task-parallel.c trie.c queue.c linkedlist.c hashtable.c aho-corasick.c
	$(CC) $(CFLAGS) aho-corasick-task-parallel.c trie.c queue.c linkedlist.c hashtable.c aho-corasick.c -o task-parallel -lpthread

data-parallel: aho-corasick-data-parallel.c trie.c queue.c linkedlist.c hashtable.c aho-corasick.c
	$(CC) $(CFLAGS) aho-corasick-data-parallel.c trie.c queue.c linkedlist.c hashtable.c aho-corasick.c -o data-parallel -lpthread