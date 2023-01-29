#include <stdlib.h>

#include "aho-corasick.h"

/* Function to convert the Trie into a DFA */
void trie_to_automaton(struct TrieNode* root) {
    /* Initializing the queue */
    struct Queue* q;
    init(q);

    /* Adding the root to the Queue */
    add(q, root);

    while (!is_empty(q)) {
        struct TrieNode* current = pop(q);

        /* Iterating over the children to create the failure links */
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            struct TrieNode* child = current->children[i];

            if (child == NULL)
                continue;

            if (current == root) {
                child->failure = root;
            } else {
                struct TrieNode* failure = current->failure;

                while (failure != root && failure->children[i] == NULL)
                    failure = failure->failure;

                if (failure->children[i] != NULL)
                    failure = failure->children[i];

                child->failure = failure;
            }

            /* Adding child in the Queue */
            add(q, child);
        }
    }
}