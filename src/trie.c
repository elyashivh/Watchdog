
#include <stddef.h>     /*size_t*/
#include <stdlib.h>     /*malloc & free*/
#include <aio.h>        /*uint32_t*/

#include "trie.h"
#include "dhcp.h"

#define LEFT (0)
#define RIGHT (1)
#define OFF (0)
#define ON (1)
#define FALSE (0)
#define TRUE (1)
#define FAIL (-1)


/*                       FORWARD DECLARATION                          */
/******************************************************************** */
static u_int32_t GetBit(u_int32_t num, size_t index);
static int UpdateIsFull(trie_node_t* node);
/********************************************************************* */


/*                           API FUNCTION                               */
/********************************************************************** */
trie_node_t* TrieCreate(trie_node_t* root)
{
    root = (trie_node_t*)malloc(sizeof(trie_node_t));
    if (root == NULL)
    {
        return NULL;
    }

    root->children[LEFT] = NULL;
    root->children[RIGHT] = NULL;
    root->is_full = FALSE;
    root->is_leaf = FALSE;

    return root;
}

trie_node_t* TrieCreateNode(size_t not_leaf)
{
    trie_node_t* node = (trie_node_t*)malloc(sizeof(trie_node_t));
    if (node == NULL)
    {
        return NULL;
    }

    node->children[LEFT] = NULL;
    node->children[RIGHT] = NULL;
    node->is_full = FALSE;
    node->is_leaf = (not_leaf == FALSE) ? TRUE: FALSE;

    return node;
}

void TrieFreeNode(trie_node_t* sub_root)
{
    if (sub_root->children[LEFT] != NULL)
    {
        TrieFreeNode(sub_root->children[LEFT]);
    }
    if (sub_root->children[RIGHT] != NULL)
    {
        TrieFreeNode(sub_root->children[RIGHT]);
    }
    free(sub_root);
}

dhcp_status_t TrieInsert(trie_node_t* root, u_int32_t* pref_ip, int bit_index)
{
    int bit = OFF;
    dhcp_status_t status = NO_IP_IN_RANGE;

    if (bit_index < 0)
    {
        root->is_full = TRUE;
        return SUCCESS;
    }
    
    bit = GetBit(*pref_ip, bit_index);

    /*case child in bit's place dosent exist- create it*/
    if (NULL == root->children[bit])
    {
        root->children[bit] = TrieCreateNode(bit_index);
        if (NULL == root->children[bit])
        {
            return MALLOC_FAIL;
        }

        status = TrieInsert(root->children[bit], pref_ip, bit_index-1);
    }

    /*case child isn't full- enter this branch*/
    else if (FALSE == root->children[bit]->is_full)
    {
        status = TrieInsert(root->children[bit], pref_ip, bit_index-1);
    }

    /*if branch is full- look for new branch*/
    if (NO_IP_IN_RANGE == status)
    {
        /*branch is full*/
        if (1 == bit)
        {
            return NO_IP_IN_RANGE;
        }
        else
        {
            *pref_ip = *pref_ip | (1 << bit_index);
            *pref_ip = *pref_ip & (~((1 << bit_index) -1));
            status = TrieInsert(root, pref_ip, bit_index);
        }

        if (MALLOC_FAIL == status)
        {
            return status;
        }
    }
            
    root->is_full = UpdateIsFull(root);

    return status;
}

trie_node_t* TrieRemove(trie_node_t* node, u_int32_t ip, int bit_index, dhcp_status_t* status)
{
    if (0 > bit_index)
    {
        free(node);
        *status = SUCCESS;
        return NULL;
    }

    /*have right child*/
    if (ON == GetBit(ip, bit_index))
    {
        /*export for static function*/
        if (NULL == node->children[RIGHT])
        {
            *status = REMOVE_FAIL;
            return node;
        }
        node->children[RIGHT] = TrieRemove(node->children[RIGHT], ip, bit_index-1, status);
    }
    else
    {
        if (NULL == node->children[LEFT])
        {
            *status = REMOVE_FAIL;
            return node;
        }
        node->children[LEFT] = TrieRemove(node->children[LEFT], ip, bit_index-1, status);
    }

    /*have no children*/
    if (NULL == node->children[LEFT] && NULL == node->children[RIGHT])
    {
        free(node);
        return NULL;
    }

    node->is_full = UpdateIsFull(node);
    return node;

}

size_t TrieLeafsCounter(trie_node_t* node)
 {
    if (NULL == node)
    {
        return 0;
    }
    else if ((node->children[LEFT] == NULL && node->children[RIGHT] == NULL))
    {
        return 1;
    }

    return TrieLeafsCounter(node->children[LEFT]) + TrieLeafsCounter(node->children[RIGHT]);
 }

/*                         STATIC FUNCTION                             */
/********************************************************************** */
static u_int32_t GetBit(u_int32_t num, size_t index)
{
	size_t mask = (size_t)1;
	size_t bit = (size_t)0;
	
	mask <<= index;
	bit = mask & num;
	return bit >> index;
}

static int UpdateIsFull(trie_node_t* node)
 {
    if (node->children[LEFT] != NULL && node->children[RIGHT] != NULL)
    {
        return node->children[LEFT]->is_full && node->children[RIGHT]->is_full;
    }

    return FALSE;
 }
