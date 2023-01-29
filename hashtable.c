#include "hashtable.h"

struct HashTable* init_hash_map() {
    struct HashTable* table = (struct HashTable*) malloc(sizeof(struct HashTable));
    int i;
    for (i = 0; i < SIZE; i++) {
        table->array[i] = NULL;
    }
    return table;
}

unsigned long hash(unsigned char* str) {
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

void put(struct HashTable* table, char* key, int value) {
    int index = hash(key) % SIZE;
    struct HashTableNode* new_node = (struct HashTableNode*) malloc(sizeof(struct HashTableNode));
    new_node->key = strdup(key);
    new_node->value = value;
    new_node->next = table->array[index];
    table->array[index] = new_node;
}

int get(struct HashTable* table, char* key) {
    int index = hash(key) % SIZE;
    struct HashTableNode* current = table->array[index];
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            return current->value;
        }
        current = current->next;
    }
    return -1;
}

void fill_hashtable(struct HashTable* table, struct LinkedList* list) {
    struct LinkedListNode* current = list->head;
    while (current != NULL) {
        put(table, current->data, 0);
        current = current->next;
    }
}