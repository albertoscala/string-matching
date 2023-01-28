#include "linkedlist.h"

struct LinkedList* init_linked_list() {
    struct LinkedList* list = (struct LinkedList*) malloc(sizeof(struct LinkedList));
    list->head = NULL;
    list->tail = NULL;
    return list;
}

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

void print_list(struct LinkedList* list) {
    struct LinkedListNode* current = list->head;
    while (current != NULL) {
        printf("%d ", current->data);
        current = current->next;
    }
}