CC = clang
CFLAGS = -Wall -g

all: m-serial m-parallel

m-serial: aho-corasick-m-serial.c
	$(CC) $(CFLAGS) aho-corasick-m-serial.c -o m-serial

m-parallel: aho-corasick-m-parallel.c
	$(CC) $(CFLAGS) aho-corasick-m-parallel.c -o m-parallel -lpthread
 
# serial: aho-corasick-serial.c trie.c queue.c linkedlist.c hashtable.c aho-corasick.c
# 	$(CC) $(CFLAGS) aho-corasick-serial.c trie.c queue.c linkedlist.c hashtable.c aho-corasick.c -o serial
# 
# parallel: aho-corasick-data-parallel.c trie.c queue.c linkedlist.c hashtable.c aho-corasick.c
# 	$(CC) $(CFLAGS) aho-corasick-data-parallel.c trie.c queue.c linkedlist.c hashtable.c aho-corasick.c -o data-parallel -lpthread
