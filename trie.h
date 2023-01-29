#ifndef TRIE_H_   /* Include guard */
#define TRIE_H_

#include <stdlib.h>
#include <stdbool.h>

/* Constant that rapresent the lengh of the alphabet in ASCII */
#define ALPHABET_SIZE 26

/* Struct that rappresent the Trie Node */
struct TrieNode {
    struct TrieNode* children[ALPHABET_SIZE];

    bool is_end_of_word;
    char *pattern;

    struct TrieNode* failure;
};

/* Function to insert a brand new node to the Trie */
struct TrieNode* insert_node();

/* Function to insert a pattern/needle into the Trie */
void insert_pattern(struct TrieNode* root, char *pattern);

#endif