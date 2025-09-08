#include <stdlib.h>		/*malloc, realloc, free*/
#include <assert.h>		/*assert*/
#include <string.h>		/*memcpy*/

#include "vector.h"

#define GROWTH_POWER (2)
#define START_CAPACITY (1)

struct vector
{
	void* items;
	size_t size_of_item;
	size_t capacity;
	size_t size;
};

vector_t* VectorCreate(size_t size_of_item)
{
	vector_t* vector = malloc(sizeof(vector_t));
	if (NULL == vector)
	{
		return NULL;
	}
	vector->size_of_item = size_of_item;
	vector->capacity = START_CAPACITY;
	vector->size = 0;
	vector->items = malloc(vector->size_of_item * START_CAPACITY);
	{
		if (NULL ==  vector->items)
		{
			free(vector);
			return NULL;
		}
	}
	return vector;
}


void VectorDestroy(vector_t* vector)
{
	assert (NULL != vector);
	free(vector->items);
	free(vector);
}


status_t VectorPushBack(vector_t* vector, void* item)
{
	assert(NULL != vector);
	assert(NULL != item);
	
	if (vector->size == vector->capacity)
	{
		vector->capacity *= GROWTH_POWER;
		vector->items = realloc(vector->items, (vector->capacity * vector->size_of_item));
		if (NULL == vector->items)
		{
			return ALLOCATION_FAIL;	
		}
	}
	memcpy(((char*)vector->items + vector->size * vector->size_of_item), item, vector->size_of_item);
	++vector->size;
	
	return SUCCESS;
}


void VectorPopBack(vector_t* vector)
{
	--vector->size;
}


size_t VectorGetSize(const vector_t* vector)
{
	assert(NULL != vector);
	return vector->size;
}


size_t VectorGetCapacity(const vector_t* vector)
{
	assert(NULL != vector);
	return vector->capacity;
}


void* VectorGetAccess(vector_t* vector, size_t index)
{
	assert(NULL != vector);
	
	return (char*)vector->items + index * vector->size_of_item;
}


status_t VectorReserve(vector_t* vector, size_t new_capacity)
{
	vector_t* tmp = NULL;
	assert(NULL != vector);
	
	if (new_capacity <= vector->capacity)
	{
		return RESIZE_FAILED;
	}
	
	tmp = realloc(vector->items, (new_capacity * vector->size_of_item));
	
	if (NULL == tmp)
	{
		return 	RESIZE_FAILED;
	}
	vector->capacity = new_capacity;
	vector->items = tmp;
	
	return SUCCESS;
}


status_t VectorShrink(vector_t* vector)
{
	assert(NULL != vector);
	vector->capacity /= GROWTH_POWER;
	vector->items = realloc(vector->items, (vector->capacity * vector->size_of_item));
	if (NULL == vector->items)
	{
		vector->capacity *= GROWTH_POWER;
		return ALLOCATION_FAIL;	
	}
	if (vector->capacity < vector->size)
	{
		 vector->size = vector->capacity;
	}
	
	return SUCCESS;
}























