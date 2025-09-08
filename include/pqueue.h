#ifndef __PQUEUE_H__
#define __PQUEUE_H__

#include <stddef.h>
/* #include "sdll.h"*/
#include "heap.h"


typedef struct pqueue pqueue_t;
typedef int(*pq_comperator_t)(void*, void*);

/*
	Description: Create a new Prority Queue.
	Args: comperator to compare between elements
	Return Value: pointer to the queue.
	Time Complexity: O(1) 
	Space Complexity: O(1)
*/
pqueue_t* PQCreate(pq_comperator_t comperator);

/*
	Description: Destroy a givven PQ
	Args: queue to destroy
	Return Value: void
	Time Complexity: O(1)
	Space Complexity: O(1)
*/
void PQDestroy(pqueue_t* queue);

/*
	Description: Insert an element to the PQ.
	Args: queue - to insert to. void* data - data to insert
	Return Value: status_t - if the operation successeded
	Time Complexity: O(n)
	Space Complexity: O(n)
*/

status_t PQEnqueue(pqueue_t* queue, void *data);

/*
	Description: Remove an element to the PQ.
	Args: queue 
	Return Value: void* - the data which get out
	Time Complexity: O(n)
	Space Complexity:O(n)
*/
void* PQDequeue(pqueue_t* queue);

/*
	Description: represent the first element in the queue
	Args: queue
	Return Value: void* - the first element (top priority)
	Time Complexity: O(1)
	Space Complexity: O(1)
*/
void* PQPeek(const pqueue_t* queue);

/*
	Description: check if the queue is empty
	Args: queue
	Return Value: 1- if is empty, 0- otherwise
	Time Complexity: O(1)
	Space Complexity: O(1)
*/
int PQIsEmpty(const pqueue_t* queue);

/*
	Description: check queue size 
	Args: queue
	Return Value: size_t which represent the queue size
	Time Complexity: O(n)
	Space Complexity: O(1)
*/
size_t PQSize(const pqueue_t* queue);

/*
	Description: Delete specific element from the queue
	Args: pqueue- the queue. data - to look for , (*IsMatch)(void*, void*) - condition for searchin
	Return Value: void* - the deleted data
	Time Complexity: O(n)
	Space Complexity: O(1)
*/
void* PQErase(pqueue_t* pqueue, void* data, int (*IsMatch)(void*, void*));

/*
	Description: Remove all queue's elements
	Args: queue
	Return Value: void
	Time Complexity: O(n)
	Space Complexity: O(1)
*/
void PQClear(pqueue_t* queue);


#endif /* end of header guard __PQUEUE_H__ */

