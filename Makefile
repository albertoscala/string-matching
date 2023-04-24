CC = clang
CFLAGS = -Wall -g

all: m-serial m-pthread m-openmp

m-serial: aho-corasick-m-serial.c
	$(CC) $(CFLAGS) aho-corasick-m-serial.c -o m-serial

m-pthread: aho-corasick-m-pthread.c
	$(CC) $(CFLAGS) aho-corasick-m-pthread.c -o m-pthread -lpthread

m-openmp: aho-corasick-m-openmp.c
	$(CC) $(CFLAGS) aho-corasick-m-openmp.c -o m-openmp -fopenmp 
 
# serial: aho-corasick-serial.c trie.c queue.c linkedlist.c hashtable.c aho-corasick.c
# 	$(CC) $(CFLAGS) aho-corasick-serial.c trie.c queue.c linkedlist.c hashtable.c aho-corasick.c -o serial
# 
# parallel: aho-corasick-data-parallel.c trie.c queue.c linkedlist.c hashtable.c aho-corasick.c
#	$(CC) $(CFLAGS) aho-corasick-data-parallel.c trie.c queue.c linkedlist.c hashtable.c aho-corasick.c -o data-parallel -lpthread
