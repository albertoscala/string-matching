#include "hashtable.h"

#define SIZE 500

struct Node {
    char* key;
    int value;
    struct Node* next;
};

struct HashTable {
    struct Node* array[SIZE];
};

struct HashTable* init_hash_map() {
    struct HashTable* table = (struct HashMap*) malloc(sizeof(struct HashTable));
    int i;
    for (i = 0; i < SIZE; i++) {
        table->array[i] = NULL;
    }
    return table;
}

unsigned long hash(unsigned char *str) {
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

void put(struct HashTable* table, char* key, int value) {
    int index = hash(key) % SIZE;
    struct Node* new_node = (struct Node*) malloc(sizeof(struct Node));
    new_node->key = strdup(key);
    new_node->value = value;
    new_node->next = table->array[index];
    table->array[index] = new_node;
}

int get(struct HashTable* table, char* key) {
    int index = hash(key) % SIZE;
    struct Node* current = table->array[index];
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            return current->value;
        }
        current = current->next;
    }
    return -1;
}

bool contains_key(struct HashTable* table, char* key) {
    int index = hash(key) % SIZE;
    struct Node* current = table->array[index];
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            return true;
        }
        current = current->next;
    }
    return false;
}