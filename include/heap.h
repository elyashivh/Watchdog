/*
    Version 1.1.0
*/

#ifndef __HEAP_H__
#define __HEAP_H__

#include <stddef.h>
#include "vector.h"

typedef int (*heap_compare_func_t)(void*, void*);
typedef int (*heap_is_match_t)(void* data, void* params);
typedef struct heap heap_t; 


/*
    Description: 
    Args: 
    Return value:       
    Time complexity: O(1)    
    Space complexity: O(1)   
*/
heap_t* HeapCreate(heap_compare_func_t compare_func);

/*
    Description: 
    Args:              
    Return value:   
    Time complexity: O(1)   
    Space complexity: O(1)   
*/
void HeapDestroy(heap_t* heap); /*O(n)*/

/*
    Description:         
    Args:       
    Return value:  
    Time complexity: O (log(n))
    Space complexity: O(1)
*/

int HeapPush(heap_t* heap, void* data); /* O (log(n)) */

/*
    Description:        
    Args:     
    Return value:   
    Time complexity: O(1)
    Space complexity:  O(1)   
*/
void* HeapPeek(const heap_t* heap); /* O(1) */

/*
    Description:        
    Args:               
    Return value:       
    Time complexity: O (log(n))
    Space complexity:  O(1)   
*/
void HeapPop(heap_t* heap);  /* O(log(n)) */

/*
    Description:        
    Args:               
    Return value:       
    Time complexity: O(n)    
    Space complexity: O(1)   
*/
void* HeapRemove(heap_t* heap, heap_is_match_t is_match, void* params); /*O(log(n))*/
/*
    Description:        
    Args:               
    Return value:       
    Time complexity: O(1)   
    Space complexity:  O(1)
*/
int HeapIsEmpty(const heap_t* heap); /* O(1) */

/*
    Description:        
    Args:               
    Return value:       
    Time complexity:   O(n) 
    Space complexity: O(1)
*/
size_t HeapSize(const heap_t* heap); /* O(n) */


#endif /* __HEAP_H__*/