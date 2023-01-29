#include <stdio.h>
#include <stdlib.h>

#include "aho-corasick.h"

void search(struct TrieNode* root, char *haystack, struct HashTable* table);

int main(void) {
    struct TrieNode* root = insert_node();

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

    fclose(text);

    FILE* patterns = fopen("test1p.txt", "r");

    if (patterns == NULL) {
        printf("Error opening file\n");
        return -1;
    }

    struct LinkedList* needles = init_linked_list();

    char* buffer = NULL;
    char* string = NULL;

    while (!feof(patterns)) {
        if (getline(&buffer, &size, patterns) != -1) {
            buffer[strcspn(buffer, "\n")] = 0;

            string = (char*) malloc(strlen(buffer) + 1);
            // copy the line to the new string
            strcpy(string, buffer);

            append(needles, string);
        } else {
            printf("Error while reading file\n");
        }
    }

    fclose(patterns);

    print_list(needles);

    return 0;
}

void search(struct TrieNode* root, char *haystack, struct HashTable* table) {
    
}