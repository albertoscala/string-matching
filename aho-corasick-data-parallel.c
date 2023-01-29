#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "aho-corasick.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct Arguments {
    struct LinkedList* haystacks;
    struct TrieNode* root;
    struct HashTable* table;
};

void haystacks_subset(struct LinkedList** datas, struct LinkedList* haystacks, int n_threads);
void* search_thread(void* args);
void search(struct LinkedList* haystacks, struct TrieNode* root, struct HashTable* table);

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

    search(haystacks, root, table);

    print_hash_table(table);

    return 0;
}

void haystacks_subset(struct LinkedList** datas, struct LinkedList* haystacks, int n_threads) {
    int next_index = 0;
    int list_size = length(haystacks);

    // Linked Lists initialization
    for (int i = 0; i < n_threads; i++) {
        datas[i] = init_linked_list();
    }

    // Dividing tasks equally between the various threads
    for (int i = 0; i < list_size; i++) {
        append(datas[next_index], get_at(haystacks, i));
        next_index = (next_index + 1) % n_threads;
    }
}

void* search_thread(void* args) {
    struct Arguments* data = (struct Arguments*) args;

    struct LinkedList* haystacks = data->haystacks;
    struct TrieNode* root = data->root;
    struct HashTable* table = data->table;

    struct TrieNode* p = root;

    int count;

    for (int i = 0; i < length(haystacks); i++) {

        char* haystack = get_at(haystacks, i);

        for (int j = 0; j < strlen(haystack); j++) {
            int idx = haystack[j] - 'a';

            while (p != root && p->children[idx] == NULL)
                p = p->failure;

            if (p->children[idx] != NULL)
                p = p->children[idx];

            if (p->is_end_of_word) {
                pthread_mutex_lock(&mutex);
                count = get(table, p->pattern);
                put(table, p->pattern, count+1);
                pthread_mutex_unlock(&mutex);
            }
        }
    }
}

void search(struct LinkedList* haystacks, struct TrieNode* root, struct HashTable* table) {
    int n_threads = sysconf(_SC_NPROCESSORS_ONLN) - 1;

    pthread_t threads[n_threads];
    struct LinkedList* datas[n_threads];

    haystacks_subset(datas, haystacks, n_threads);
    
    /* Test to check is tasks are correcly distributed */
    /*
    for (int i = 0; i < n_threads; i++) {
        printf("Datas %d\n", i);
        print_list(datas[i]);
    }
    */
    struct Arguments* args;

    for (int i = 0; i < n_threads; i++) {
        args = (struct Arguments*) malloc(sizeof(struct Arguments));
        args->haystacks = datas[i];
        args->root = root;
        args->table = table;

        pthread_create(&threads[i], NULL, search_thread, args);
        pthread_join(threads[i], NULL);
    }
}