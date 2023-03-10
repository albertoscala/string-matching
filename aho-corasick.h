#ifndef AHO_CORASICK_H_   /* Include guard */
#define AHO_CORASICK_H_

#include "trie.h"
#include "queue.h"
#include "linkedlist.h"
#include "hashtable.h"

/* Function to convert the Trie into a DFA */
void trie_to_automaton(struct TrieNode* root);

#endif