#include <stdio.h>
#include <stdlib.h>

#include "aho-corasick.h"

/* Function to search the patterns */
void search(struct TrieNode* root, struct LinkedList* haystacks, struct HashTable* table);

int main(void) {
    /* Creating the root of the Trie */
    struct TrieNode* root = insert_node();

    /* Reading the haystacks from the file */
    FILE* text = fopen("testa-1000000-10t.txt", "r");

    if (text == NULL) {
        printf("Error opening file\n");
        return -1;
    }

    size_t size = 0;

    char* buffer = NULL;
    char* string = NULL;

    struct LinkedList* haystacks = init_linked_list();

    while (!feof(text)) {
        if (getline(&buffer, &size, text) != -1) {
            buffer[strcspn(buffer, "\n")] = 0;

            string = (char*) malloc(strlen(buffer) + 1);
            
            /* copy the line to the new string */
            strcpy(string, buffer);

            append(haystacks, string);
        } else {
            printf("Error while reading texts file\n");
            return -1;
        }
    }

    printf("Texts read correcly\n");

    fclose(text);

    /* Reading the needles from the file */
    FILE* patterns = fopen("testa-500000-11p.txt", "r");

    if (patterns == NULL) {
        printf("Error opening file\n");
        return -1;
    }

    struct LinkedList* needles = init_linked_list();

    while (!feof(patterns)) {
        if (getline(&buffer, &size, patterns) != -1) {
            buffer[strcspn(buffer, "\n")] = 0;

            string = (char*) malloc(strlen(buffer) + 1);
            
            /* Copy the line to the new string */
            strcpy(string, buffer);

            append(needles, string);
        } else {
            printf("Error while reading patterns file\n");
            return -1;
        }
    }

    printf("Patterns read correcly\n");

    fclose(patterns);

    /* Creating the HashTable */
    struct HashTable* table = init_hash_table();

    /* Filling the HashTable with the patterns and initializing the counters to zero */
    fill_hashtable(table, needles);

    /* Creating the Trie given the LinkedList with the needles */
    create_trie(root, needles);

    /* Creating the failure pointers transforming the Trie into a DFA */
    trie_to_automaton(root);

    search(root, haystacks, table);

    print_hash_table(table);

    return 0;
}

/* Function to search the patterns */
void search(struct TrieNode* root, struct LinkedList* haystacks, struct HashTable* table) {
    struct TrieNode* p;

    int count;

    for (int i = 0; i < length(haystacks); i++) {

        /* Resetting the pointer of the Trie to the root */
        p = root;

        /* Getting the next haystack to analyze */
        char* haystack = get_at(haystacks, i);

        for (int j = 0; j < strlen(haystack); j++) {
            /* Getting the index for the Trie children */
            int idx = haystack[j] - 'a';

            /* Next expected char not found */
            while (p != root && p->children[idx] == NULL)
                p = p->failure;

            /* Success continue */
            if (p->children[idx] != NULL)
                p = p->children[idx];

            /* If pattern is found */
            if (p->is_end_of_word) {
                count = get(table, p->pattern);
                put(table, p->pattern, count+1);
            }
        }
    }
}