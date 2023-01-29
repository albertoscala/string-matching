#include <stdio.h>
#include <stdlib.h>

#include "aho-corasick.h"

void search(struct TrieNode* root, struct LinkedList* haystacks, struct HashTable* table);

int main(void) {
    struct TrieNode* root = insert_node();

    FILE* text = fopen("test3t.txt", "r");

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
            // copy the line to the new string
            strcpy(string, buffer);

            append(haystacks, string);
        } else {
            printf("Error while reading file\n");
        }
    }

    printf("Texts read correcly\n");

    fclose(text);

    FILE* patterns = fopen("test3p.txt", "r");

    if (patterns == NULL) {
        printf("Error opening file\n");
        return -1;
    }

    struct LinkedList* needles = init_linked_list();

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

    search(root, haystacks, table);

    print_hash_table(table);

    return 0;
}

void search(struct TrieNode* root, struct LinkedList* haystacks, struct HashTable* table) {
    struct TrieNode* p;

    int count;

    for (int i = 0; i < length(haystacks); i++) {

        p = root;
        char* haystack = get_at(haystacks, i);

        for (int j = 0; j < strlen(haystack); j++) {
            int idx = haystack[j] - 'a';

            while (p != root && p->children[idx] == NULL)
                p = p->failure;

            if (p->children[idx] != NULL)
                p = p->children[idx];

            if (p->is_end_of_word) {
                count = get(table, p->pattern);
                printf("Trovato %s a riga %d il contatore è a %d e sarà a %d\n", p->pattern, i, count, count+1);
                put(table, p->pattern, count+1);
            }
        }
    }
}