#include "linkedlist.h"

/* Function that initialize the Linked List */
struct LinkedList* init_linked_list() {
    struct LinkedList* list = (struct LinkedList*) malloc(sizeof(struct LinkedList));
    list->head = NULL;
    list->tail = NULL;
    return list;
}

/* Function that appends a string to the Linked List */
void append(struct LinkedList* list, char* data) {
    struct LinkedListNode* new_node = (struct LinkedListNode*) malloc(sizeof(struct LinkedListNode));
    new_node->data = data;
    new_node->next = NULL;

    if (list->head == NULL) {
        list->head = new_node;
        list->tail = new_node;
        return;
    }

    list->tail->next = new_node;
    list->tail = new_node;
}

/* Function that prints the Linked List content */
void print_list(struct LinkedList* list) {
    struct LinkedListNode* current = list->head;

    while (current != NULL) {
        printf("%s\n", current->data);
        current = current->next;
    }
}

/* Function that returns the length of the Linked List */
int length(struct LinkedList* list) {
    int length = 0;
    struct LinkedListNode* current = list->head;

    while (current != NULL) {
        length = length + 1;
        current = current->next;
    }

    return length;
}

/* Function that returns the value of the Linked List at certain position */
char* get_at(struct LinkedList* list, int pos) {
    int iter = 0;
    struct LinkedListNode* current = list->head;

    while (current != NULL) {
        if (iter == pos) {
            return current->data;
        }
        current = current->next;
        iter = iter + 1;
    }

    return NULL;
}

/* Function that returns if a value is already in the Linked List */
bool is_in_list(struct LinkedList* list, char *value) {
    struct LinkedListNode* current = list->head;
    while (current != NULL) {
        if (strcmp(current->data, value) == 0) {
            return true;
        }
        current = current->next;
    }
    return false;
}