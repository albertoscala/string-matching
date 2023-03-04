#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define ALPHABET_SIZE 26

struct TrieNode {
    struct TrieNode* children[ALPHABET_SIZE];
    bool is_end_of_word;
    char* pattern;
    struct TrieNode* failure;
    struct TrieNode* output;
};

struct TrieNode* insert_node();

void insert_pattern(struct TrieNode* root, char* pattern);

struct Queue {
    struct QueueNode* head;
    struct QueueNode* tail;
};

struct QueueNode {
    struct TrieNode* data;
    struct QueueNode* next;
};

struct Queue* init();

bool is_empty(struct Queue* q);

void enqueue(struct Queue* q, struct TrieNode* node);

struct TrieNode* dequeue(struct Queue* q);

void build_failure_links(struct TrieNode* root);

void add_output_links(struct TrieNode* root);

void search(struct TrieNode* root, char* haystack);

int main(int argc, char* argv[]) {
    int h_size = 0, n_size = 0;

    size_t size = 0;
    char* buffer = NULL;
    char* string = NULL;
    
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

    size = 0;
    buffer = NULL;
    string = NULL;

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

    struct TrieNode* root = insert_node();

    for (int i = 0; i < n_size; i++) {
        printf("Pattern %d: %s\n", i, needles[i]);
        insert_pattern(root, needles[i]);
    }

    build_failure_links(root);

    add_output_links(root);

    for (int i = 0; i < h_size; i++)
        search(root, haystacks[i]);

    return 0;
}

struct TrieNode* insert_node() {
    struct TrieNode* p = (struct TrieNode *) malloc(sizeof(struct TrieNode));

    for (int i = 0; i < ALPHABET_SIZE; i++)
        p->children[i] = NULL;

    p->is_end_of_word = false;

    p->pattern = NULL;

    p->failure = NULL;

    p->output = NULL;

    return p;
}

void insert_pattern(struct TrieNode* root, char* pattern) {
    struct TrieNode* p = root;

    for (int i = 0; i < strlen(pattern); i++) {
        int idx = pattern[i] - 'a';

        if (!p->children[idx])
            p->children[idx] = insert_node();

        p = p->children[idx];
    }

    p->is_end_of_word = true;

    p->pattern = pattern;
}

struct Queue* init() {
    struct Queue* q = (struct Queue*) malloc(sizeof(struct Queue));
    
    q->head = NULL;
    q->tail = NULL;
    
    return q;
}

bool is_empty(struct Queue* q) {
    if (q->head == NULL)
        return true;

    return false;
}

void enqueue(struct Queue* q, struct TrieNode* node) {
    struct QueueNode* new_node = (struct QueueNode*) malloc(sizeof(struct QueueNode));
    new_node->data = node;
    new_node->next = NULL;

    if (q->tail == NULL) {
        q->head = q->tail = new_node;
        return;
    }

    q->tail->next = new_node;
    q->tail = new_node;
}

struct TrieNode* dequeue(struct Queue* q) {
    struct QueueNode* temp = q->head;
    struct TrieNode* node = temp->data;
    q->head = temp->next;

    if (q->head == NULL)
        q->tail = NULL;

    free(temp);

    return node;
}

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

void search(struct TrieNode* root, char* haystack) {
    int n = strlen(haystack);
    struct TrieNode* curr = root;

    for (int i = 0; i < n; i++) {
        int index = haystack[i] - 'a';

        while (curr != root && !curr->children[index]) {
            curr = curr->failure;
        }

        if (curr->children[index]) {
            curr = curr->children[index];
        }

        if (curr->is_end_of_word)
            printf("Pattern found: %s\n", curr->pattern);

        if (curr->output) {
            // A pattern match was found
            printf("Pattern found: %s\n", curr->output->pattern);
        }

        struct TrieNode* suffix_node = curr->failure;
        while (suffix_node) {
            if (suffix_node->output) {
                // A pattern match was found
                printf("Pattern found: %s\n", suffix_node->output->pattern);
            }
            suffix_node = suffix_node->failure;
        }
    }
}