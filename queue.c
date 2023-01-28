#include "queue.h"

/* Structs that rappresent the Queue */
struct Node {
    struct TrieNode* data;
    struct Node* next;
};

struct Queue {
    struct Node* head;
    struct Node* tail;
};

/* Function to initialize the Queue */
void init(struct Queue* q) {
    q->head = NULL;
    q->tail = NULL;
}

/* Function to check if the Queue is empty */
int is_empty(struct Queue* q) {
    return (q->head == NULL);
}

/* Function to add an element to the back of the Queue */
void add(struct Queue* q, struct TrieNode* node) {
    struct Node* new_node = (struct Node*) malloc(sizeof(struct Node));
    new_node->data = node;
    new_node->next = NULL;

    if (q->tail == NULL) {
        q->head = q->tail = new_node;
        return;
    }

    q->tail->next = new_node;
    q->tail = new_node;
}

/* Function to pop/return and delete the element at the head of the Queue */
struct TrieNode* pop(struct Queue* q) {
    if (is_empty(q)) {
        printf("Queue is empty\n");
        return -1;
    }

    struct Node* temp = q->head;
    struct TrieNode* node = temp->data;
    q->head = temp->next;

    if (q->head == NULL)
        q->tail = NULL;

    free(temp);

    return node;
}