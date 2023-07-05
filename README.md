# String Matching

In this repository I'm going to show my multicore programming course project, which consisted of implementing a string matching algorithm to find the patterns in the strings received from a socket and later on improve the algorthm performances using a multithread programmig library and applying the techniques to make a serial program into a parallel one. In this case I decided to implement the Aho-Corasick because it was the most interesting and efficent one because of its particular data structure

## Building the Trie

The first step is to build the Trie.

Create a root node, then go through each pattern character by character. Starting from the root, try and follow an edge for the current character. If one does not exist, create an edge to a new node for that character.

Using P = {vine, vincent, cent, center}, say we start with the pattern 'vine'. When we go and add the next pattern 'vincent', we first follow the existing edges for v-i-n, and then add in new edges for c-e-n-t:

![Example 1](https://user-images.githubusercontent.com/31989626/222902315-33d2076f-02f1-4d35-bf16-35eb69020dc8.png)

(I'm marking the ends of each pattern with a double circle). Continue this process until the whole trie is built:

![Example 2](https://user-images.githubusercontent.com/31989626/222902343-9ca00437-d664-446a-b9f9-2685aac4ab0a.png)

### Code implementation

The implementation of the Trie data structure was relatively straightforward.

To begin, we require two elements in the node structure: a list of children and a variable to indicate whether the node marks the end of a pattern (leaf). With these considerations in mind, I devised the following C struct for the implementation.

```c
#define ALPHABET_SIZE 26

struct TrieNode {
    struct TrieNode* children[ALPHABET_SIZE];
    bool is_end_of_word;
};
```

Now, we need to create two functions: one for generating the nodes of our Trie and another for populating the actual structure.

The following function enables the creation of our nodes by allocating memory for a node and initializing it.

```c
struct TrieNode* insert_node() {
    struct TrieNode* p = (struct TrieNode *) malloc(sizeof(struct TrieNode));

    for (int i = 0; i < ALPHABET_SIZE; i++)
        p->children[i] = NULL;

    p->is_end_of_word = false;

    return p;
}
```

Next, we have the function that enables us to populate the Trie with the patterns we need to search for. This function takes the root of the Trie and the pattern to search for as arguments. We iterate character by character through the pattern, adding and creating edges in our Trie if they do not already exist. Finally, when we reach the end of the word, we mark the last node as a leaf.

```c
void insert_pattern(struct TrieNode* root, char* pattern) {
    struct TrieNode* p = root;

    for (int i = 0; i < strlen(pattern); i++) {
        int idx = pattern[i] - 'a';

        if (!p->children[idx])
            p->children[idx] = insert_node();

        p = p->children[idx];
    }

    p->is_end_of_word = true;
}
```

## Adding Failure Links

Now to add the suffix links, we perform a breadth first search of the tree, adding an edge for each node except the root. If a node is one hop away from the root, its suffix link goes to the root. Adding suffix links in blue, the suffix links for the nodes corresponding to 'v' and 'c' are:

![Example 3](https://user-images.githubusercontent.com/31989626/222902379-d5869f86-4b38-4521-acca-aa498bb96dd3.png)

Otherwise, the node corresponds to some string *wx*, where *x* is the final character. For example, if we look at the string 'vi', *w* ='v' and *x* ='i'. Now follow *w*'s suffix link and let the node you arrive at be called *n*.

![Example 4](https://user-images.githubusercontent.com/31989626/222902401-a85357d3-66d2-4f97-b30d-ee9ba3edaecf.png)

If this node has an edge for *x*, then set *wx* 's suffix link to point at *nx*. Else if *n* is the root node, set  *wx*'s suffix link to point at *n*. Otherwise, follow *n*'s suffix link, let this node be the new *n*, and repeat. In this case, the node for *n* does not have an edge for 'i'. Here's another example adding in the suffix link for 'vinc', where *w*='vin' and *x*='c':

![Example 5](https://user-images.githubusercontent.com/31989626/222902414-88d59816-4c38-4337-ad4c-a47dca47cb3d.png)

### Code implementation

 Before proceeding with the implementation of failure links, we need to introduce a new field in our Trie node struct: a link to another Trie node that we will refer to as "failure." As a result, our struct should be updated to include this additional field. Here is the modified struct representation:

```c
struct TrieNode {
    struct TrieNode* children[ALPHABET_SIZE];
    bool is_end_of_word;
    struct TrieNode* failure;   /* Failure link */
};
```

Certainly, it is important to handle the new field carefully and make appropriate adjustments in the previously implemented functions to account for it.

To implement the failure links, we need to use a breadth-first search (BFS) approach. The first step is to obtain a queue. In this case, I have implemented my own Queue data structure.

For each element in the queue, we iterate over its children and follow two fundamental rules:

1. If the parent of the child is the root, the failure link will point to the root.
2. Otherwise, we trace the failure links of the current node being examined until either the failure link reaches the root or the child of the failure link becomes a valid node.

By following these rules, we determine the appropriate node to point to in case of failure.

This process is repeated for every node in the queue until the queue becomes empty.

It is crucial to ensure that all functions are updated to handle the new field and incorporate the logic for failure links accordingly.

```c
void build_failure_links(struct TrieNode* root) {
    struct Queue* q = init();

    enqueue(q, root);

    while (!is_empty(q)) {
        struct TrieNode* current = dequeue(q);

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

            enqueue(q, child);
        }
    }
}
```

## Adding Output Links

As you are performing the BFS to fill in the suffix links, you can also fill in the output links (all initially null). Let the current node be *n*. Follow *n*'s suffix link and let the node you arrive at be called *m*.

If *m* corresponds to one of the pattern strings (is marked with a double circle), set *n*'s output link to point at *m*. Else set *n*'s output link to point at the same node as *m*'s output link, or null if *m* has no output link. In our example there is only one output link, shown here in green:

![Example 6](https://user-images.githubusercontent.com/31989626/222902439-2df025ca-6d40-4922-9ba3-bfb7b5b5c3a5.png)

### Code implementation

This part is quite similar to the previous step. Once again, we perform a breadth-first search (BFS) over the graph and follow the failure link of each current node. Here's how it works:

If the current node, along with its failure link, points to the end of a word, we set the output link of the current node to that particular node. However, if the current node and its failure link do not point to the end of a word, we set the output link of the current node to be the same as the output link of the failure link.

```c
void add_output_links(struct TrieNode* root) {
    /* Initializing the queue */
    struct Queue* q = init();

    /* Adding the root to the Queue */
    enqueue(q, root);

    while (!is_empty(q)) {
        struct TrieNode* current = dequeue(q);

        /* Iterating over the children to create the failure links */
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            struct TrieNode* child = current->children[i];

            if (child == NULL)
                continue;

            if (child->failure->is_end_of_word) {
                child->output = child->failure;
            } else {
                child->output = child->failure->output;
            }

            /* Adding child in the Queue */
            enqueue(q, child);
        }
    }
}
```

## Writing the Search

Now, we reach the final part of the algorithm, where we utilize the constructed structure to find matches within our text.

Before diving into the code implementation, let's analyze the different scenarios in which we encounter a match:

1. The first occasion is when the current character represents the end of a pattern.
2. The second occasion is when the current character has an output link, indicating a complete match of a pattern.
3. The last occasion is when we traverse the failure links, and one of them leads to an output link, signifying a match.

These three cases cover the various situations in which matches can be found during the search process. By identifying these matches, we can accurately track the occurrence of patterns within the given text.

Now, armed with this understanding, we can proceed to implement the search function in the code editor.

### Code implementation

To begin, we iterate through each character in the `haystack` string.

First, we check if the next character exists as a child node in the Trie. If not, we backtrack through the Trie by following failure links until we either reach the root or find a node with the existing child.

If the child node exists, we update the current node (`curr`) pointer to point to the child.

Next, we perform two checks to see if we have encountered a match:

1. We verify if the current character marks the end of a word (pattern).
2. We check if the current node has an output link, indicating a complete match of a pattern.

Finally, we enter a `while` loop to traverse the failure links. We check each node along the failure links to see if they have any output links. This process continues until we reach the root of the Trie.

By following these steps, we efficiently search for matches in the `haystack` string using the Aho-Corasick algorithm. The use of failure links optimizes the search process and ensures accurate identification of pattern matches throughout the Trie structure.

```c
void search(struct TrieNode* root, char* haystack, int* counters) {
    int n = strlen(haystack);
    struct TrieNode* curr = root;

    /* Iterating over the string to analyze */
    for (int i = 0; i < n; i++) {
        int index = haystack[i] - 'a';

        while (curr != root && !curr->children[index]) {
            curr = curr->failure;
        }

        if (curr->children[index])
            curr = curr->children[index];

        /* Pattern match was found */
        if (curr->is_end_of_word)
            counters[curr->id] = counters[curr->id] + 1;

        /* Pattern match was found */ 
        if (curr->output)
            counters[curr->output->id] = counters[curr->output->id] + 1;

        struct TrieNode* suffix_node = curr->failure;
        while (suffix_node) {
            /* Pattern match was found */
            if (suffix_node->output)
                counters[suffix_node->output->id] = counters[suffix_node->output->id] + 1;
  
            suffix_node = suffix_node->failure;
        }
    }
}
```

## Parallelization

### Problem explanation and solution


### Implementation

#### Pthread


```c
void search(struct TrieNode* root, char** haystacks, int h_size, int* counters) {
    /* Getting the number of threads available on the machine */
    int n_threads = sysconf(_SC_NPROCESSORS_ONLN) - 1;

    int l_threads = (h_size < n_threads) ? h_size : n_threads;

    pthread_t threads[l_threads];

    struct Arguments* args;

    int part_size = h_size / l_threads;
    int remainder = h_size % l_threads;
    int start_idx = 0, end_idx = 0;

    for(int i = 0; i < l_threads; i++) {
        end_idx += part_size;

        if (remainder > 0) {
            end_idx++;
            remainder--;
        }

        args = malloc(sizeof(struct Arguments));

        args->root = root;
        args->haystacks = haystacks;
        args->haystacks_start = start_idx;
        args->haystacks_end = end_idx;
        args->counters = counters;

        pthread_create(&threads[i], NULL, threaded_search, args);

        start_idx = end_idx;
    }
  
    for (int i = 0; i < l_threads; i++) {
        pthread_join(threads[i], NULL);
    }
}
```



```c
void* threaded_search(void* args) {
    struct Arguments* pa = (struct Arguments*) args;

    struct TrieNode* root = pa->root;

    char** haystacks = pa->haystacks;
    int haystacks_start = pa->haystacks_start;
    int haystacks_end = pa->haystacks_end;

    // printf("Thread creato n tasks %d start %d end %d\n", haystacks_end - haystacks_start, haystacks_start, haystacks_end);

    int* counters = pa->counters;

    for (int h = haystacks_start; h < haystacks_end; h++) {
        char* haystack = haystacks[h];
  
        int n = strlen(haystack);
        struct TrieNode* curr = root;

        /* Iterating over the string to analyze */
        for (int i = 0; i < n; i++) {
            int index = haystack[i] - 'a';

            while (curr != root && !curr->children[index]) {
                curr = curr->failure;
            }

            if (curr->children[index])
                curr = curr->children[index];

            /* Pattern match was found */
            if (curr->is_end_of_word) {
                pthread_mutex_lock(&lock);
                counters[curr->id] = counters[curr->id] + 1;
                // printf("Pattern found: %s\n", curr->pattern);
                pthread_mutex_unlock(&lock);
            }

            /* Pattern match was found */ 
            if (curr->output) {
                pthread_mutex_lock(&lock);
                counters[curr->output->id] = counters[curr->output->id] + 1;
                // printf("Pattern found: %s\n", curr->output->pattern);
                pthread_mutex_unlock(&lock);
            }

            struct TrieNode* suffix_node = curr->failure;
            while (suffix_node) {
                /* Pattern match was found */
                if (suffix_node->output) {
                    pthread_mutex_lock(&lock);
                    counters[suffix_node->output->id] = counters[suffix_node->output->id] + 1;
                    // printf("Pattern found: %s\n", suffix_node->output->pattern);
                    pthread_mutex_unlock(&lock);
                }
            
                suffix_node = suffix_node->failure;
            }
        }
    }

    pthread_exit(NULL);
}
```

#### OpenMP


```c
// main function
#   pragma omp parallel for
    for (int i = 0; i < h_size; i++)
        search(root, haystacks[i], counters);
```


```c
void search(struct TrieNode* root, char* haystack, int* counters) {
    int n = strlen(haystack);
    struct TrieNode* curr = root;

    /* Iterating over the string to analyze */
    for (int i = 0; i < n; i++) {
        int index = haystack[i] - 'a';

        while (curr != root && !curr->children[index]) {
            curr = curr->failure;
        }

        if (curr->children[index])
            curr = curr->children[index];

        /* Pattern match was found */
        if (curr->is_end_of_word) {
    
#           pragma omp critical
            {
                counters[curr->id] = counters[curr->id] + 1;
            }
            //printf("Pattern found: %s\n", curr->pattern);
        }

        /* Pattern match was found */ 
        if (curr->output) {

#           pragma omp critical
            {
                counters[curr->output->id] = counters[curr->output->id] + 1;
            }
            // printf("Pattern found: %s\n", curr->output->pattern);
        }

        struct TrieNode* suffix_node = curr->failure;
        while (suffix_node) {
            /* Pattern match was found */
            if (suffix_node->output) {

#               pragma omp critical
                {
                    counters[suffix_node->output->id] = counters[suffix_node->output->id] + 1;
                }
                // printf("Pattern found: %s\n", suffix_node->output->pattern);
            }
        
            suffix_node = suffix_node->failure;
        }
    }
}
```



## License

[MIT](https://choosealicense.com/licenses/mit/)
