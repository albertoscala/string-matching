#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/time.h>
 
/*
 * Max number of states in the matching machine.
 * Should be equal to the sum of the length of all keywords.
 */
#define MAXS 500
 
/* Characters in input alphabet */
#define MAXC 26

/* Number of strings to analyze */
#define HSTRN 2

/* Length of strings to analyze */
#define HSTRL 10

/* Number of strings to search for */
#define NSTRN 4

/* Length of strings to search for */
#define NSTRL 10

/*
 * OUTPUT FUNCTION
 * Bit i in this mask is one of the word with index i
 * appears when the machine enters this state.
 */
int out[MAXS];
 
/* FAILURE FUNCTION */
int f[MAXS];
 
/* GOTO FUNCTION (TRIE) */
int g[MAXS][MAXC];

/* Structs that represent the Queue */
struct Queue {
    struct QueueNode* head;
    struct QueueNode* tail;
};

struct QueueNode {
    int data;
    struct QueueNode* next;
};

/* Function to initialize the Queue */
struct Queue* init();

/* Function to check if the Queue is empty */
bool is_empty(struct Queue* q);

/* Function to add an element to the Queue */
void enqueue(struct Queue* q, int data);

/* Function to remove the element at the head of the Queue */
int dequeue(struct Queue* q);

// Builds the string matching machine.
// arr -   array of words. The index of each keyword is important:
//         "out[state] & (1 << i)" is > 0 if we just found word[i]
//         in the text.
// Returns the number of states that the built machine has.
// States are numbered 0 up to the return value - 1, inclusive.
int build_ac_graph(char needles[NSTRN][NSTRL], int n_size)
{
    // Initialize all values in g function as 0.
    memset(out, 0, sizeof(out));
 
    // Initialize all values in goto function as -1.
    memset(g, -1, sizeof(g));
 
    // Initially, we just have the 0 state
    int states = 1;
 
    // Construct values for goto function, i.e., fill g[][]
    // This is same as building a Trie for arr[]
    for (int i = 0; i < n_size; ++i) {

        char* needle = needles[i];

        int current_state = 0;
 
        // Insert all characters of current word in arr[]
        for (int j = 0; j < strlen(needle); ++j) {
            int ch = needle[j] - 'a';

            // Allocate a new node (create a new state) if a
            // node for ch doesn't exist.
            if (g[current_state][ch] == -1)
                g[current_state][ch] = states++;

            current_state = g[current_state][ch];
        }
 
        // Add current word in g function
        out[current_state] |= (1 << i);
    }
 
    // For all characters which don't have an edge from
    // root (or state 0) in Trie, add a goto edge to state
    // 0 itself
    for (int ch = 0; ch < MAXC; ++ch)
        if (g[0][ch] == -1)
            g[0][ch] = 0;
 
    // Now, let's build the f function
 
    // Initialize values in fail function
    memset(f, -1, sizeof(f));
 
    // Failure function is computed in breadth first order
    // using a queue
    struct Queue* q = init();
 
     // Iterate over every possible input
    for (int ch = 0; ch < MAXC; ch++) {
        // All nodes of depth 1 have f function value
        // as 0. For example, in above diagram we move to 0
        // from states 1 and 3.
        if (g[0][ch] != 0) {
            f[g[0][ch]] = 0;
            enqueue(q, g[0][ch]);
        }
    }
 
    // Now queue has states 1 and 3
    while (!is_empty(q)) {
        // Remove the front state from queue
        int state = dequeue(q);
 
        // For the removed state, find f function for
        // all those characters for which goto function is
        // not defined.
        for (int ch = 0; ch <= MAXC; ch++) {
            // If goto function is defined for character 'ch'
            // and 'state'
            if (g[state][ch] != -1) {
                // Find f state of removed state
                int failure = f[state];
 
                // Find the deepest node labeled by proper
                // suffix of string from root to current
                // state.
                while (g[failure][ch] == -1)
                      failure = f[failure];
 
                failure = g[failure][ch];
                f[g[state][ch]] = failure;
 
                // Merge g values
                out[g[state][ch]] |= out[failure];
 
                // Insert the next level node (of Trie) in Queue
                enqueue(q, g[state][ch]);
            }
        }
    }
 
    return states;
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
void enqueue(struct Queue* q, int data) {
    /* Create a new node for the Queue */
    struct QueueNode* new_node = (struct QueueNode*) malloc(sizeof(int));
    new_node->data = data;
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
int dequeue(struct Queue* q) {
    struct QueueNode* temp = q->head;
    int node = temp->data;
    
    /* Set the head to the second element */
    q->head = temp->next;

    /* Special case: last element of the Queue */
    if (q->head == NULL)
        q->tail = NULL;

    free(temp);

    return node;
}

// Returns the next state the machine will transition to using goto
// and f functions.
// currentState - The current state of the machine. Must be between
//                0 and the number of states - 1, inclusive.
// nextInput - The next character that enters into the machine.
int find_next_state(int currentState, char nextInput) {

    int answer = currentState;
    int ch = nextInput - 'a';
 
    /* If goto function is not defined, use f function */
    while (g[answer][ch] == -1)
        answer = f[answer];
 
    return g[answer][ch];
}
 
/* This function finds all the occurrences */
void search(int n_size, char haystacks[HSTRN][HSTRL], int* counters) {

    /* Initialize current state */
    int current_state = 0;

    /* Iterate for each haystack */
    for (int i = 0; i < HSTRN; i++) {

        /* Traverse the text through the built machine to find all occurrences */
        for (int j = 0; j < strlen(haystacks[i]); j++) {

            current_state = find_next_state(current_state, haystacks[i][j]);

            /* If match not found, move to next state */
            if (out[current_state] == 0)
                continue;

            /* Match found, update patterns counter. */
            for (int k = 0; k < n_size; k++) {
                if (out[current_state] & (1 << k)) {
                    counters[k] += 1;
                }
            }
        }
    }
}
 
// Driver program to test above
int main(int argc, char** argv) {

    int n_size = 4, h_size = 1;

    int* counters = calloc(n_size, sizeof(int));

    char needles[NSTRN][NSTRL];

    strcpy(needles[0], "he");
    strcpy(needles[1], "she");
    strcpy(needles[2], "hers");
    strcpy(needles[3], "his");

    char haystacks[HSTRN][HSTRL];

    strcpy(haystacks[0], "ahishers");
    strcpy(haystacks[1], "ahishers");

    /* Building the AC graph */
    build_ac_graph(needles, n_size);

    search(n_size, haystacks, counters);

    for (int i = 0; i < n_size; i++)
        printf("Needle %s found %d times\n", needles[i], counters[i]);
 
    return 0;
}