#ifndef LINKED_LIST_H_   /* Include guard */
#define LINKED_LIST_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

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

/* Function that returns the length of the Linked List */
int length(struct LinkedList* list);

/* Function that returns the value of the Linked List at certain position */
char* get_at(struct LinkedList* list, int pos);

/* Function that returns if a value is already in the Linked List */
bool is_in_list(struct LinkedList* list, char *value);

#endif