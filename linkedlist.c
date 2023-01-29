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
        printf("%s\n", current->data);
        current = current->next;
    }
}

int length(struct LinkedList* list) {
    int length = 0;
    struct LinkedListNode* current = list->head;
    while (current != NULL) {
        length = length + 1;
        current = current->next;
    }
    return length;
}

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