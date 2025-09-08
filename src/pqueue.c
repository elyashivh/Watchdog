/*	Author: Elyashiv Horden
	Date:	22-12-24
	CR: 	Roy Goldhar*/

#include <stddef.h>		/*size_t*/
#include <stdlib.h>		/*malloc & free*/
#include <assert.h>		/*assert*/

#include "pqueue.h"
#include "heap.h"

#define FAIL (1)

struct pqueue
{
	heap_t* heap;
};


/*                       FORWARD DECLARATION                          */
/******************************************************************** */
/* static sdll_t* GetSdll(const pqueue_t* pqueue);
static void SetSdll(pqueue_t* pqueue, sdll_t* sdll); */
static heap_t* GetHeap(const pqueue_t* pqueue);

/********************************************************************* */



/*                           API FUNCTION                               */
/********************************************************************** */
pqueue_t* PQCreate(pq_comperator_t comperator)
{
	pqueue_t* pqueue = NULL;
	
	pqueue = (pqueue_t*)malloc(sizeof(pqueue_t));
	if (!pqueue)
	{
		return NULL;
	}
	
	pqueue->heap = HeapCreate(comperator);
	if (NULL == pqueue->heap)
	{
		free(pqueue);
		return NULL;
	} 
	
	return pqueue;
}

void PQDestroy(pqueue_t* pqueue)
{
	assert(pqueue);
	
	HeapDestroy(GetHeap(pqueue));

	free(pqueue);
}

status_t PQEnqueue(pqueue_t* pqueue, void *data)
{
	assert(pqueue);

	if (FAIL == HeapPush(GetHeap(pqueue), data))
	{
		return FAIL;
	}

	return SUCCESS;
}


void* PQDequeue(pqueue_t* pqueue)
{
	void* ans = NULL;
	assert(pqueue);
	assert(!PQIsEmpty(pqueue));
	
	ans = PQPeek(pqueue);
	HeapPop(GetHeap(pqueue));

	return ans;
}


void* PQPeek(const pqueue_t* pqueue)
{
	assert(pqueue);
	assert(!PQIsEmpty(pqueue));
	
	return HeapPeek(GetHeap(pqueue));

}


int PQIsEmpty(const pqueue_t* pqueue)
{
	assert(pqueue);
	
	return HeapIsEmpty(GetHeap(pqueue));
}


size_t PQSize(const pqueue_t* pqueue)
{
	assert(pqueue);
	
	return HeapSize(GetHeap(pqueue));
}


void* PQErase(pqueue_t* pqueue, void* data, int (*IsMatch)(void*, void*))
{
	assert(pqueue);
	
	return HeapRemove(GetHeap(pqueue), IsMatch, data);
}


void PQClear(pqueue_t* pqueue)
{	
	assert(pqueue);
	
	while(!HeapIsEmpty(GetHeap(pqueue)))
	{
		HeapPop(GetHeap(pqueue));
	}
} 


static heap_t* GetHeap(const pqueue_t* pqueue) 
{
	assert(pqueue);
	
	return pqueue->heap;
}







