#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>

#define ALPHABET_SIZE 26

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

/* Struct that rappresent the Trie Node */
struct TrieNode {
    struct TrieNode* children[ALPHABET_SIZE];
    bool is_end_of_word;
    char* pattern;
    int id;
    struct TrieNode* failure;   /* Failure link */
    struct TrieNode* output;    /* Output link */
};

/* Function to insert a brand new node to the Trie */
struct TrieNode* insert_node();

/* Function to insert a pattern/needle into the Trie */
void insert_pattern(struct TrieNode* root, char* pattern, int id);

/* Structs that rappresent the Queue */
struct Queue {
    struct QueueNode* head;
    struct QueueNode* tail;
};

struct QueueNode {
    struct TrieNode* data;
    struct QueueNode* next;
};

/* Function to initialize the Queue */
struct Queue* init();

/* Function to check if the Queue is empty */
bool is_empty(struct Queue* q);

/* Function to add an element to the Queue */
void enqueue(struct Queue* q, struct TrieNode* node);

/* Function to remove the element at the head of the Queue */
struct TrieNode* dequeue(struct Queue* q);

/* Function to fill the failure links */
void build_failure_links(struct TrieNode* root);

/* Function to fill the output links */
void add_output_links(struct TrieNode* root);

struct Arguments {
    struct TrieNode* root;
    char** haystacks;
    int haystacks_n;
    int* counters;
    int counters_n;
};

/* Function to prepare the search for the threads */
void search(struct TrieNode* root, char** haystacks, int h_size, int* counters);

/* Function to search the patterns */
void* threaded_search(void* args);

int main(int argc, char* argv[]) {
    int* counters;
    int h_size = 0, n_size = 0;

    size_t size = 0;
    char* buffer = NULL;
    char* string = NULL;
    
    /* Start reading texts file */
    FILE* texts = fopen(argv[1], "r");
    
    if (texts == NULL) {
        printf("Error opening texts file\n");
        return -1;
    }  

    char** haystacks = (char**) malloc(sizeof(char*));

    while (!feof(texts)) {
        h_size = h_size + 1;

        if (getline(&buffer, &size, texts) != -1) {

            haystacks = realloc(haystacks, h_size * sizeof(char*));
            
            buffer[strcspn(buffer, "\n")] = 0;

            string = (char*) malloc(strlen(buffer) + 1);
            
            /* copy the line to the new string */
            strcpy(string, buffer);

            haystacks[h_size - 1] = string;
        } else {
            printf("Error while reading texts file\n");
            return -1;
        }
    }

    fclose(texts);
    /* End reading texts file */

    size = 0;
    buffer = NULL;
    string = NULL;

    /* Start reading patterns file */
    FILE* patterns = fopen(argv[2], "r");

    if (patterns == NULL) {
        printf("Error opening patterns file\n");
        return -1;
    }

    char** needles = (char**) malloc(sizeof(char*));

    while (!feof(patterns)) {
        n_size = n_size + 1;

        if (getline(&buffer, &size, patterns) != -1) {

            needles = realloc(needles, n_size * sizeof(char*));
            
            buffer[strcspn(buffer, "\n")] = 0;

            string = (char*) malloc(strlen(buffer) + 1);
            
            /* copy the line to the new string */
            strcpy(string, buffer);

            needles[n_size - 1] = string;
        } else {
            printf("Error while reading texts file\n");
            return -1;
        }
    }

    fclose(patterns);
    /* End reading patterns file */

    /* Create the Trie */
    struct TrieNode* root = insert_node();

    counters = calloc(n_size, sizeof(int));

    /* Fill the Trie with the patterns */
    for (int i = 0; i < n_size; i++) {
        printf("Pattern %d: %s\n", i, needles[i]);
        insert_pattern(root, needles[i], i);
    }

    build_failure_links(root);

    add_output_links(root);

    search(root, haystacks, h_size, counters);

    //TODO: Make the print for the counters

    return 0;
}

/* Function to insert a brand new node to the Trie */
struct TrieNode* insert_node() {
    struct TrieNode* p = (struct TrieNode *) malloc(sizeof(struct TrieNode));

    for (int i = 0; i < ALPHABET_SIZE; i++)
        p->children[i] = NULL;

    p->is_end_of_word = false;

    p->pattern = NULL;

    p->id = NULL;

    p->failure = NULL;

    p->output = NULL;

    return p;
}

/* Function to insert a pattern/needle into the Trie */
void insert_pattern(struct TrieNode* root, char* pattern, int id) {
    struct TrieNode* p = root;

    for (int i = 0; i < strlen(pattern); i++) {
        int idx = pattern[i] - 'a';

        if (!p->children[idx])
            p->children[idx] = insert_node();

        p = p->children[idx];
    }

    p->id;

    p->is_end_of_word = true;

    p->pattern = pattern;
}

/* Function to initialize the Queue */
struct Queue* init() {
    struct Queue* q = (struct Queue*) malloc(sizeof(struct Queue));
    
    q->head = NULL;
    q->tail = NULL;
    
    return q;
}

/* Function to check if the Queue is empty */
bool is_empty(struct Queue* q) {
    if (q->head == NULL)
        return true;

    return false;
}

/* Function to add an element to the Queue */
void enqueue(struct Queue* q, struct TrieNode* node) {
    /* Create a new node for the Queue */
    struct QueueNode* new_node = (struct QueueNode*) malloc(sizeof(struct QueueNode));
    new_node->data = node;
    new_node->next = NULL;

    /* Special case: first element of the Queue */
    if (q->tail == NULL) {
        q->head = q->tail = new_node;
        return;
    }

    /* Add at the tail of the Queue */
    q->tail->next = new_node;
    q->tail = new_node;
}

/* Function to remove the element at the head of the Queue */
struct TrieNode* dequeue(struct Queue* q) {
    struct QueueNode* temp = q->head;
    struct TrieNode* node = temp->data;
    
    /* Set the head to the second element */
    q->head = temp->next;

    /* Special case: last element of the Queue */
    if (q->head == NULL)
        q->tail = NULL;

    free(temp);

    return node;
}

/* Function to fill the failure links */
void build_failure_links(struct TrieNode* root) {
    /* Initializing the queue */
    struct Queue* q = init();

    /* Adding the root to the Queue */
    enqueue(q, root);

    while (!is_empty(q)) {
        struct TrieNode* current = dequeue(q);

        /* Iterating over the children to create the failure links */
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            struct TrieNode* child = current->children[i];

            if (child == NULL)
                continue;

            if (current == root) {
                child->failure = root;
            } else {
                struct TrieNode* failure = current->failure;

                while (failure != root && failure->children[i] == NULL)
                    failure = failure->failure;

                if (failure->children[i] != NULL)
                    failure = failure->children[i];

                child->failure = failure;
            }

            /* Adding child in the Queue */
            enqueue(q, child);
        }
    }
}

void add_output_links(struct TrieNode* root) {
    /* Initializing the queue */
    struct Queue* q = init();

    /* Adding the root to the Queue */
    enqueue(q, root);

    while (!is_empty(q)) {
        struct TrieNode* current = dequeue(q);

        /* Iterating over the children to create the failure links */
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            struct TrieNode* child = current->children[i];

            if (child == NULL)
                continue;

            if (child->failure->is_end_of_word) {
                child->output = child->failure;
            } else {
                child->output = child->failure->output;
            }

            /* Adding child in the Queue */
            enqueue(q, child);
        }
    }
}

void search(struct TrieNode* root, char** haystacks, int h_size, int* counters) {
    /* Getting the number of threads available on the machine */
    int n_threads = sysconf(_SC_NPROCESSORS_ONLN) - 1;

    int* haystacks_subsets[n_threads];

    // TODO: Trova un modo per splittare

    int part_size = h_size / n_threads;
    int remainder = h_size % n_threads;
    int start_idx = 0;
    
    // SI ma fallo per le stringhe. Usa crea l'array e poi passalo subito e successivamente deallocalo dal thread
    for(int i = 0; i < n_threads; i++) {
        if (i == n_threads - 1 && remainder > 0) {
            haystacks_subsets[i] = malloc((h_size - start_idx + part_size) * sizeof(int));
            for (int j = start_idx + part_size, k=0; j < h_size; j++, k++) {
                haystacks_subsets[i][k] = haystacks[j];
            }
        } else {
            haystacks_subsets[i] = malloc(part_size * sizeof(int));
            for (int j = start_idx, k=0; j < start_idx + part_size; j++, k++) {
                haystacks_subsets[i][k] = haystacks[j];
            }
        }

        start_idx += part_size;
    }
    
    for (int i = 0; i < n_threads; i++) {
        printf("Array n. %d\n", i);
        for (int j = 0; j < sizeof(haystacks_subsets[i])/sizeof(haystacks_subsets[i][0]); j++) {
            printf("%d\n", )
        }
    }

    return 0;
}

void* threaded_search(void* args) {
    struct Arguments* pa = (struct Arguments*) args;

    struct TrieNode* root = pa->root;

    char** haystacks = pa->haystacks;
    int haystacks_n = pa->haystacks_n;

    int* counters = pa->counters;
    int counters_n = pa ->counters_n;

    for (int h = 0; h < haystacks_n; h++) {
        char* haystack = haystacks[h];
    
        int n = strlen(haystack);
        struct TrieNode* curr = root;

        /* Iterating over the string to analyze */
        for (int i = 0; i < n; i++) {
            int index = haystack[i] - 'a';

            while (curr != root && !curr->children[index]) {
                curr = curr->failure;
            }

            if (curr->children[index])
                curr = curr->children[index];

            /* Pattern match was found */
            if (curr->is_end_of_word) {
                pthread_mutex_lock(&lock);
                counters[curr->id] = counters[curr->id] + 1;
                // printf("Pattern found: %s\n", curr->pattern);
                pthread_mutex_unlock(&lock);
            }

            /* Pattern match was found */ 
            if (curr->output) {
                pthread_mutex_lock(&lock);
                counters[curr->output->id] = counters[curr->output->id] + 1;
                // printf("Pattern found: %s\n", curr->output->pattern);
                pthread_mutex_unlock(&lock);
            }

            struct TrieNode* suffix_node = curr->failure;
            while (suffix_node) {
                /* Pattern match was found */
                if (suffix_node->output) {
                    pthread_mutex_lock(&lock);
                    counters[suffix_node->output->id] = counters[suffix_node->output->id] + 1;
                    // printf("Pattern found: %s\n", suffix_node->output->pattern);
                    pthread_mutex_unlock(&lock);
                }
                
                suffix_node = suffix_node->failure;
            }
        }
    }
}