#include <stdio.h>
#include <stdlib.h>

#include "aho-corasick.h"

void search(struct TrieNode* root, char *haystack, struct HashTable* table);

int main(void) {
    TrieNode* root = insert_node();

    FILE* text = fopen("test1t.txt", "r");

    if (text == NULL) {
        printf("Error opening file\n");
        return -1;
    }

    size_t size = 0;
    char* haystack = NULL;

    if (getline(&haystack, &size, text) != -1) {
        printf("Haystack value: %s\n", haystack);
    } else {
        printf("Error while reading file\n");
    }

    FILE* patterns = fopen("test1p.txt", "r");

    if (patterns == NULL) {
        printf("Error opening file\n");
        return -1;
    }

    LinkedList* needles = init_linked_list();

    char* buffer = NULL;

    if (getline(&buffer, &size, text) != -1) {
        
    } else {
        printf("Error while reading file\n");
    }


    return 0;
}

void search(struct TrieNode* root, char *haystack, struct HashTable* table) {
    
}