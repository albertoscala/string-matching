#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

#include "trie.h"
#include "linkedlist.h"
#include "hashtable.h"
#include "aho-corasick.h"

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

/* Mutex to handle the writing in the Hash Table */
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct TrieArgs {
    struct TrieNode* root; 
    struct LinkedList* patterns;
};

/* Structure that rapresent the arguments to pass to the search_thread function */
struct Arguments {
    struct LinkedList* haystacks;
    struct TrieNode* root;
    struct HashTable* table;
};

void* new_pattern(void* args);

void new_trie(struct TrieNode* root, struct LinkedList* patterns);

/* Function to split the work load equaly based on the number of threads available in the machine */
void haystacks_subset(struct LinkedList** datas, struct LinkedList* haystacks, int n_threads);

/* Threaded function for the search */
void* search_thread(void* args);

/* Function to handle the search of the patterns */
void search(struct LinkedList* haystacks, struct TrieNode* root, struct HashTable* table);

int main(int argc, char *argv[]) {

    /* Taking files as arguments */
    char* text_file = argv[1];
    char* patterns_file = argv[2];

    /* Creating the root of the Trie */
    struct TrieNode* root = insert_node();

    /* Reading the haystacks from the file */
    FILE* text = fopen(text_file, "r");

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

            /* Copy the line to the new string */
            strcpy(string, buffer);

            append(haystacks, string);
        } else {
            printf("Error while reading file\n");
        }
    }

    printf("Texts read correcly\n");

    fclose(text);

    /* Reading the needles from the file */
    FILE* patterns = fopen(patterns_file, "r");

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
            printf("Error while reading file\n");
        }
    }

    printf("Patterns read correcly\n");

    fclose(patterns);

    /* Creating the HashTable */
    struct HashTable* table = init_hash_table();

    /* Filling the HashTable with the patterns and initializing the counters to zero */
    fill_hashtable(table, needles);

    /* Creating the Trie given the LinkedList with the needles */
    new_trie(root, needles);

    /* Creating the failure pointers transforming the Trie into a DFA */
    trie_to_automaton(root);

    search(haystacks, root, table);

    print_hash_table(table);

    return 0;
}

/* Function to split the work load equaly based on the number of threads available in the machine */
void haystacks_subset(struct LinkedList** datas, struct LinkedList* haystacks, int n_threads) {
    int next_index = 0;
    int list_size = length(haystacks);

    /* LinkedLists initialization */
    for (int i = 0; i < n_threads; i++) {
        datas[i] = init_linked_list();
    }

    /* Dividing tasks equally between the various threads */
    for (int i = 0; i < list_size; i++) {
        append(datas[next_index], get_at(haystacks, i));
        next_index = (next_index + 1) % n_threads;
    }
}

/* Threaded function for the search */
void* search_thread(void* args) {
    struct Arguments* data = (struct Arguments*) args;

    /* Unpacking the structure in singles variables */
    struct LinkedList* haystacks = data->haystacks;
    struct TrieNode* root = data->root;
    struct HashTable* table = data->table;

    struct TrieNode* p = root;

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
                pthread_mutex_lock(&mutex);
                count = get(table, p->pattern);
                put(table, p->pattern, count+1);
                pthread_mutex_unlock(&mutex);
            }
        }
    }
}

/* Function to handle the search of the patterns */
void search(struct LinkedList* haystacks, struct TrieNode* root, struct HashTable* table) {
    /* Getting the number of threads available on the machine */
    int n_threads = sysconf(_SC_NPROCESSORS_ONLN) - 1;

    pthread_t threads[n_threads];
    struct LinkedList* datas[n_threads];

    /* Splitting the work */
    haystacks_subset(datas, haystacks, n_threads);
    
    struct Arguments* args;

    /* Creating and joining the threads */
    for (int i = 0; i < n_threads; i++) {
        args = (struct Arguments*) malloc(sizeof(struct Arguments));
        args->haystacks = datas[i];
        args->root = root;
        args->table = table;

        pthread_create(&threads[i], NULL, search_thread, args);
        pthread_join(threads[i], NULL);
    }
}

/* Function to insert a pattern/needle into the Trie */
void* new_pattern(void* args) {
    
    struct TrieArgs* arguments = (struct TrieArgs*) args;

    struct TrieNode* root = arguments->root; 
    struct LinkedList* patterns = arguments->patterns;
    
    struct TrieNode* p = root;
    struct LinkedListNode* current = patterns->head;

    while (current != NULL) {
        char* pattern = current->data;
        pthread_mutex_lock(&lock);
        for (int i = 0; i < strlen(pattern); i++) {
            int idx = pattern[i] - 'a';

            if (!p->children[idx])
                p->children[idx] = insert_node();

            p = p->children[idx];
        }

        p->is_end_of_word = true;
        p->pattern = pattern;
        pthread_mutex_unlock(&lock);
        current = current->next;
    }
}

/* Creates the Trie given a Linked List of strings */
void new_trie(struct TrieNode* root, struct LinkedList* patterns) {

    struct LinkedList* datas[11];
    pthread_t threads[11];

    haystacks_subset(datas, patterns, 11);

    struct TrieArgs* args;

    for (int i=0; i<11; i++) {
        args = (struct TrieArgs*) malloc(sizeof(struct TrieArgs));
        args->root = root;
        args->patterns = datas[i];

        pthread_create(&threads[i], NULL, new_pattern, args);
        pthread_join(threads[i], NULL);
    }
}

struct TrieNode* insert_node() {
    struct TrieNode* p = (struct TrieNode *) malloc(sizeof(struct TrieNode));

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        p->children[i] = NULL;
    }

    p->is_end_of_word = false;

    p->failure = NULL;

    return p;
}  