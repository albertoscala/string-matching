#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "aho-corasick.h"

struct Arguments {
    char* haystack;
    struct LinkedList* needles;
    struct HashTable* table;
};

void patterns_subset(struct LinkedList** tasks, struct LinkedList* needles, int n_threads);
void* search_thread(void* args);
void search(char* haystack, struct LinkedList* needles, struct HashTable* table);

int main(void) {
    struct TrieNode* root = insert_node();

    FILE* text = fopen("test2t.txt", "r");

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

    FILE* patterns = fopen("test2p.txt", "r");

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

            if (!is_in_list(needles, buffer)) {    

                string = (char*) malloc(strlen(buffer) + 1);
                // copy the line to the new string
                strcpy(string, buffer);

                append(needles, string);
            }
        } else {
            printf("Error while reading file\n");
        }
    }

    printf("Patterns read correcly\n");

    fclose(patterns);

    struct HashTable* table = init_hash_table();

    fill_hashtable(table, needles);

    search(haystack, needles, table);

    print_hash_table(table);

    return 0;
}

void patterns_subset(struct LinkedList** tasks, struct LinkedList* needles, int n_threads) {
    int next_index = 0;
    int list_size = length(needles);

    // Linked Lists initialization
    for (int i = 0; i < n_threads; i++) {
        tasks[i] = init_linked_list();
    }

    // Dividing tasks equally between the various threads
    for (int i = 0; i < list_size; i++) {
        append(tasks[next_index], get_at(needles, i));
        next_index = (next_index + 1) % n_threads;
    }
}

void* search_thread(void* args) {
    struct Arguments* data = (struct Arguments*) args;

    char* haystack = data->haystack;
    struct LinkedList* needles = data->needles;
    struct HashTable* table = data->table;

    struct TrieNode* root = insert_node();
    create_trie(root, needles);

    trie_to_automaton(root);

    struct TrieNode* p = root;

    int count;

    for (int i = 0; i < strlen(haystack); i++) {
        int idx = tolower(haystack[i]) - 'a';

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

void search(char* haystack, struct LinkedList* needles, struct HashTable* table) {
    int n_threads = sysconf(_SC_NPROCESSORS_ONLN) - 1;

    pthread_t threads[n_threads];
    struct LinkedList* tasks[n_threads];

    patterns_subset(tasks, needles, n_threads);
    
    /* Test to check is tasks are correcly distributed */
    /*
    for (int i = 0; i < n_threads; i++) {
        printf("Tasks %d\n", i);
        print_list(tasks[i]);
    }
    */
    struct Arguments* args;

    for (int i = 0; i < n_threads; i++) {
        args = (struct Arguments*) malloc(sizeof(struct Arguments));
        args->haystack = haystack;
        args->needles = tasks[i];
        args->table = table;

        pthread_create(&threads[i], NULL, search_thread, args);
        pthread_join(threads[i], NULL);
    }
}