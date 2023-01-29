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
        printf("Haystack read correcly\n");
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

    printf("Patterns read correcly\n");

    fclose(patterns);

    struct HashTable* table = init_hash_table();

    fill_hashtable(table, needles);

    create_trie(root, needles);

    trie_to_automaton(root);

    search(root, haystack, table);

    print_hash_table(table);

    return 0;
}

void search(struct TrieNode* root, char* haystack, struct HashTable* table) {
    int count;
    struct TrieNode* p = root;

    for (int i = 0; i < strlen(haystack); i++) {
        int idx = haystack[i] - 'a';

        while (p != root && p->children[idx] == NULL)
            p = p->failure;

        if (p->children[idx] != NULL)
            p = p->children[idx];

        if (p->is_end_of_word) {
            count = get(table, p->pattern);
            put(table, p->pattern, count+1);
        }
    }
}