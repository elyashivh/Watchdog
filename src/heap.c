/*
Author:     Elyashiv Horden  
Date:		?-?-2024	    
CR: 	     	
*/

#include <stdlib.h>     /*malloc & free*/
#include <stddef.h>     /*size_t*/
#include <assert.h>     /*assert*/
#include <string.h>     /*memcpy*/

#include "heap.h"
#include "vector.h"

#define SUCCESS (0)
#define FAIL (1)
#define TRUE (1)
#define FALSE (0)
#define PARENT(i) (((i) - 1) / 2)
#define RIGHT(i) ((i) * 2 + 2)
#define LEFT(i) ((i) * 2 + 1)
#define DATA(i) VectorGetAccess(heap->arr, (i))

struct heap
{
    vector_t* arr;
    heap_compare_func_t compare_func;
};


/*                       FORWARD DECLARATION                          */
/******************************************************************** */
static void DownHeapify(heap_t* heap, size_t index); 
static void UpHeapify(heap_t* heap, size_t index);
static int MakeComparison(heap_t* heap, size_t i, size_t j);
static void GenericSwap(void* a, void* b, size_t num_of_bytes);

/********************************************************************* */



/*                           API FUNCTION                               */
/********************************************************************** */

heap_t* HeapCreate(heap_compare_func_t compare_func)
{
    heap_t* heap = NULL;

    assert(compare_func);
    
    heap = (heap_t*)malloc(sizeof(heap_t));
    if (NULL == heap)
    {
        return NULL;
    }

    heap->arr = VectorCreate(sizeof(void*));
    if (NULL == heap->arr)
    {
        free(heap);
        return NULL;
    }

    heap->compare_func = compare_func;

    return heap;
}

void HeapDestroy(heap_t* heap)
{
    assert(heap);

    VectorDestroy(heap->arr);
    free(heap);
}

int HeapPush(heap_t* heap, void* data)
{
    int ans = SUCCESS;
    assert(heap);
    assert(data);

    ans = VectorPushBack(heap->arr, &data);
    if (FAIL == ans)
    {
        return ans;
    }

    UpHeapify(heap, VectorGetSize(heap->arr)-1);

    return ans;
}

void* HeapPeek(const heap_t* heap)
{
    assert(heap);

    return *(void**)DATA(0);
}

void HeapPop(heap_t* heap)
{
    void* first_element = NULL;
    void* last_element = NULL;
    assert(heap);

    first_element = DATA(0);
    last_element = DATA(VectorGetSize(heap->arr)-1);
    memcpy(first_element, last_element, sizeof(void*));
    VectorPopBack(heap->arr);

    DownHeapify(heap, 0);
}

void* HeapRemove(heap_t* heap, heap_is_match_t is_match, void* params)
{
    size_t i = 0;
    size_t heap_size = 0;
    void* ans = NULL;

    assert(heap);
    assert(is_match);

    heap_size = VectorGetSize(heap->arr);
    for (i = 0; i < heap_size; ++i)
    {   
        if (is_match(*(void**)DATA(i), params))
        {
            ans = *(void**)DATA(i);
            GenericSwap(DATA(i), DATA(heap_size-1), sizeof(void*));
            VectorPopBack(heap->arr);
            DownHeapify(heap, i);
            return ans;
        }
    }
    return NULL;
}

int HeapIsEmpty(const heap_t* heap)
{
    assert(heap);

    return VectorGetSize(heap->arr) == 0;
}

size_t HeapSize(const heap_t* heap)
{
    assert(heap);

    return VectorGetSize(heap->arr);
}



/*                         STATIC FUNCTION                              */
/********************************************************************** */
static void DownHeapify(heap_t* heap, size_t index) /*O (log n)*/
{
    size_t i = index;
    size_t heap_size = HeapSize(heap);

    /*Have left child & child is smaller than his father*/
    if (LEFT(index) < heap_size && MakeComparison(heap, LEFT(index), i) < 0)
    {
        i = LEFT(index);
    }
    /*Have right child & child is smaller than his father*/
    if (RIGHT(index) < heap_size && MakeComparison(heap, RIGHT(index), i) < 0)
    {
        i = RIGHT(index);
    }
    if (index != i)
    {
        GenericSwap(DATA(i), DATA(index), sizeof(void*));
        DownHeapify(heap, i);
    }
}

static void UpHeapify(heap_t* heap, size_t index) /*O (log n)*/
{
    if (0 == index || MakeComparison(heap, PARENT(index), index) <= 0)
    {
        return;
    }

    else
    {
        GenericSwap(DATA(index), DATA(PARENT(index)), sizeof(void*));
        UpHeapify(heap, PARENT(index));
    }
}

static int MakeComparison(heap_t* heap, size_t i, size_t j)
{
    return heap->compare_func(*(void**)DATA(i), *(void**)DATA(j));
}

static void GenericSwap(void* a, void* b, size_t num_of_bytes)
{
    size_t i = 0;
    char temp = 0;

    for (i = 0; i < num_of_bytes; ++i)
    {
        temp = *(char*)a;
        *(char*)a = *(char*)b;
        *(char*)b = temp;
        a = (char*)a + 1;
        b = (char*)b + 1;
    }
}

