#include "hashtable.h"

struct HashTable* init_hash_table() {
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
    unsigned long hash_val = hash(key) % SIZE;
    struct HashTableNode* current = table->array[hash_val];
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            current->value = value;
            return;
        }
        current = current->next;
    }
    struct HashTableNode* newNode = (struct HashTableNode*)malloc(sizeof(struct HashTableNode));
    newNode->key = key;
    newNode->value = value;
    newNode->next = table->array[hash_val];
    table->array[hash_val] = newNode;
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

void print_hash_table(struct HashTable* table) {
    struct HashTableNode* temp;

    for (int i = 0; i < SIZE; i++) {
        temp = table->array[i];
        printf("Bucket %d: ", i);

        while (temp != NULL) {
            printf("(%s, %d) ", temp->key, temp->value);
            temp = temp->next;
        }

        printf("\n");
    }
}