#ifndef LINKED_LIST_H_   /* Include guard */
#define LINKED_LIST_H_

#include <stdio.h>
#include <stdlib.h>

/* Structs that rapresent the Linked List */
struct LinkedListNode {
    char* data;
    struct LinkedListNode* next;
};

struct LinkedList {
    struct LinkedListNode* head;
    struct LinkedListNode* tail;
};

/* Function that initialize the Linked List */
struct LinkedList* init_linked_list();

/* Function that appends a string to the Linked List */
void append(struct LinkedList* list, char* data);

/* Function that prints the Linked List content */
void print_list(struct LinkedList* list);

#endif