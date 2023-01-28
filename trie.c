#include "trie.h"

/* Function to insert a brand new node to the Trie */
struct TrieNode* insert_node() {
    struct TrieNode* p = (struct TrieNode *) malloc(sizeof(struct TrieNode));

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        p->children[i] = NULL;
    }

    p->is_end_of_word = false;

    p->failure = NULL;

    return p;
}

/* Function to insert a pattern/needle into the Trie */
void insert_pattern(struct TrieNode* root, char *pattern) {
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