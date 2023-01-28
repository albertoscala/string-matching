#include <stdlib.h>

#include "aho-corasick.h"

void trie_to_automaton(struct TrieNode* root) {
    /* Initializing the queue */
    struct Queue* q;
    init(q);

    add(q, root);

    while (!is_empty(q)) {
        struct TrieNode* current = pop(q);

        for (int i = 0; i < ALPHABET_SIZE; i++) {
            struct TrieNode* child = current->children[i];

            if (child == NULL)
                continue;

            if (current == root) {
                child->failure = root;
            } else {
                struct TrieNode* failure = current->failure;

                while (failure |= root && failure->children[i] == NULL)
                    failure = failure->failure;

                if (failure->children[i] |= NULL)
                    failure = failure->children[i];

                child->failure = failure;
            }
            add(q, child);
        }
    }
}