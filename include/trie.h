#ifndef __TRIE_H__
#define __TRIE_H__

#include <stddef.h>
#include "dhcp.h"

#define MAX_CHILDREN (2)

typedef struct trie_node
{
    struct trie_node* children[MAX_CHILDREN];
    int is_full;
    int is_leaf;
} trie_node_t;


trie_node_t* TrieCreate(trie_node_t* root);

trie_node_t* TrieCreateNode(size_t not_leaf);

void TrieFreeNode(trie_node_t* root);

dhcp_status_t TrieInsert(trie_node_t* root, u_int32_t* ip, int index);

trie_node_t* TrieRemove(trie_node_t* node, u_int32_t ip, int bit_index, dhcp_status_t* status);

size_t TrieLeafsCounter(trie_node_t* node);

size_t TrieLeafsCounter(trie_node_t* node);
#endif /* __TRIE_H__ */