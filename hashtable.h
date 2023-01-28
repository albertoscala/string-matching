#ifndef HASH_TABLE_H_   /* Include guard */
#define HASH_TABLE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* Structs that rapresent the Hash Table */
struct Node;
struct HashTable;

/* Function that initialize the Hash Table */
struct HashTable* init_hash_map();

/* Hash function implementing djb2 hashing method */
unsigned long hash(unsigned char *str);

/* Function to put a new key in the Hash Table */
void put(struct HashTable* map, char* key, int value);

/* Function to get a value give the key from the Hash Table */
int get(struct HashTable* map, char* key);

/* Function that checks if a key is already in a Hash Table */
bool contains_key(struct HashTable* table, char* key);

#endif