# String Matching

## Building the Trie

The first step is to build a Trie for the pattern strings.

We start creating a root node, then go through each pattern character by character. Starting from the root, try and follow an edge for the current character. If one does not exist, create an edge to a new node for that character.

Using P = {vine, vincent, cent, center}, say we start with the pattern 'vine'. When we go and add the next pattern 'vincent', we first follow the existing edges for v-i-n, and then add in new edges for c-e-n-t:

![Example 1](https://user-images.githubusercontent.com/31989626/222902315-33d2076f-02f1-4d35-bf16-35eb69020dc8.png)

(The ends of the patterns are marked with a double circle). Continue this process until the whole Trie is built:

![Example 2](https://user-images.githubusercontent.com/31989626/222902343-9ca00437-d664-446a-b9f9-2685aac4ab0a.png)

## Add Failure Links

Now to add the failure links, we perform a BFS (Breadth First Search) of the tree, adding an edge for each node except the root. If a node is one hop away from the root, its failure link goes to the root. Adding failure links in blue, the failure links for the nodes corresponding to 'v' and 'c' are:

![Example 3](https://user-images.githubusercontent.com/31989626/222902379-d5869f86-4b38-4521-acca-aa498bb96dd3.png)

Otherwise, the node corresponds to some string wx, where x is the final character. For example, if we look at the string 'vi', w='v' and x='i'. Now follow w's failure link and let the node you arrive at be called n.

![Example 4](https://user-images.githubusercontent.com/31989626/222902401-a85357d3-66d2-4f97-b30d-ee9ba3edaecf.png)

If this node has an edge for x, then set wx's failure link to point at nx. Else if n is the root node, set wx's failure link to point at n. Otherwise, follow n's failure link, let this node be the new n, and repeat. In this case, the node for n does not have an edge for 'i'. Here's another example adding in the failure link for 'vinc', where w='vin' and x='c':

![Example 5](https://user-images.githubusercontent.com/31989626/222902414-88d59816-4c38-4337-ad4c-a47dca47cb3d.png)

## Add Output Links

As you are performing the BFS to fill in the failure links, you can also fill in the output links (all initially null). Let the current node be n. Follow n's failure link and let the node you arrive at be called m.

If m corresponds to one of the pattern strings (is marked with a double circle), set n's output link to point at m. Else set n's output link to point at the same node as m's output link, or null if m has no output link. In our example there is only one output link, shown here in green:

![Example 6](https://user-images.githubusercontent.com/31989626/222902439-2df025ca-6d40-4922-9ba3-bfb7b5b5c3a5.png)

## Writing the Search
