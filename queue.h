#ifndef QUEUE_H_   /* Include guard */
#define QUEUE_H_

#include <stdio.h>

#include "trie.h"

/* Structs that rappresent the Queue */
struct QueueNode {
    struct TrieNode* data;
    struct Node* next;
};

struct Queue {
    struct QueueNode* head;
    struct QueueNode* tail;
};

/* Function to initialize the Queue */
void init(struct Queue* q);

/* Function to check if the Queue is empty */
int is_empty(struct Queue* q);

/* Function to add an element to the back of the Queue */
void add(struct Queue* q, struct TrieNode* node);

/* Function to pop/return and delete the element at the head of the Queue */
struct TrieNode* pop(struct Queue* q);

#endif