#ifndef HASH_TABLE_H_   /* Include guard */
#define HASH_TABLE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "linkedlist.h"

#define SIZE 500

/* Structs that rapresent the Hash Table */
struct HashTableNode {
    char* key;
    int value;
    struct HashTableNode* next;
};

struct HashTable {
    struct HashTableNode* array[SIZE];
};

/* Function that initialize the Hash Table */
struct HashTable* init_hash_map();

/* Hash function implementing djb2 hashing method */
unsigned long hash(unsigned char *str);

/* Function to put a new key in the Hash Table */
void put(struct HashTable* map, char* key, int value);

/* Function to get a value give the key from the Hash Table */
int get(struct HashTable* map, char* key);

/* Function that fills the Hash Table with a Linked List */
void fill_hashtable(struct HashTable* table, struct LinkedList* list);

#endif