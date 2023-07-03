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

The Trie data structure implementation was pretty easy.

Starting from the struct of a node we need two things: a list of children, a variable that indicates that the node is the end of a pattern (leaf). So I came up with this C struct.

```c
#define ALPHABET_SIZE 26

struct TrieNode {
    struct TrieNode* children[ALPHABET_SIZE];
    bool is_end_of_word;
};
```

Now we have to make a function to create the nodes of our Trie and another function to fill the actual structure.

The following function is the one that allows the creation of our nodes, it simply allocate the memory for a node and initializate it

```c
struct TrieNode* insert_node() {
    struct TrieNode* p = (struct TrieNode *) malloc(sizeof(struct TrieNode));

    for (int i = 0; i < ALPHABET_SIZE; i++)
        p->children[i] = NULL;

    p->is_end_of_word = false;

    return p;
}
```

Next we have the function that allow us to fill the Trie with the patterns we have to search for. This time we give the root of the Trie and the pattern to search for as arguments to the function. Then we iterate char by char over the pattern to add and create the edges of our Trie if they don't really exist and when we are at the end of the word we flag the last node as leaf.

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

Now to add the failure links, we perform a BFS (Breadth First Search) of the tree, adding an edge for each node except the root. If a node is one hop away from the root, its failure link goes to the root. Adding failure links in blue, the failure links for the nodes corresponding to 'v' and 'c' are:

![Example 3](https://user-images.githubusercontent.com/31989626/222902379-d5869f86-4b38-4521-acca-aa498bb96dd3.png)

Otherwise, the node corresponds to some string wx, where x is the final character. For example, if we look at the string 'vi', w='v' and x='i'. Now follow w's failure link and let the node you arrive at be called n.

![Example 4](https://user-images.githubusercontent.com/31989626/222902401-a85357d3-66d2-4f97-b30d-ee9ba3edaecf.png)

If this node has an edge for x, then set wx's failure link to point at nx. Else if n is the root node, set wx's failure link to point at n. Otherwise, follow n's failure link, let this node be the new n, and repeat. In this case, the node for n does not have an edge for 'i'. Here's another example adding in the failure link for 'vinc', where w='vin' and x='c':

![Example 5](https://user-images.githubusercontent.com/31989626/222902414-88d59816-4c38-4337-ad4c-a47dca47cb3d.png)

### Code implementation

Before going to implement the failure links we have to add a new field in our Trie node struct a link to another Trie node that we will call failure so now our struct should look like this

```c
struct TrieNode {
    struct TrieNode* children[ALPHABET_SIZE];
    bool is_end_of_word;
    struct TrieNode* failure;   /* Failure link */
};
```

Of course you will have to be careful and handle the new field also in the previous implemented functions

Now to implement the failure links we have to the BFS, so the first thing is to get a Queue, in this case I implemented my own Queue.

Now for each element of the queue we iterate over their children and we follow two basic rules:

- If the parent of the child is root then the failure link will point to root
- Else we follow the failure links of the current node taken in exam until the failure link is the actual root or until the child of the failure is an actual valid node

At the end of that process with find the node to point to in case of failure.

Now we have to repeat it for every node in the queue until is empty

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

As you are performing the BFS to fill in the failure links, you can also fill in the output links (all initially null). Let the current node be n. Follow n's failure link and let the node you arrive at be called m.

If m corresponds to one of the pattern strings (is marked with a double circle), set n's output link to point at m. Else set n's output link to point at the same node as m's output link, or null if m has no output link. In our example there is only one output link, shown here in green:

![Example 6](https://user-images.githubusercontent.com/31989626/222902439-2df025ca-6d40-4922-9ba3-bfb7b5b5c3a5.png)

### Code implementation

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

### Code implementation

## Parallelization

### Problems

### Solution

## License

[MIT](https://choosealicense.com/licenses/mit/)
