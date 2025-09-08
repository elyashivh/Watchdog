#include <stdlib.h>		/*malloc & free*/
#include <assert.h>		/*assert*/
#include <string.h>		/*memcpy*/

#include "stack.h"

struct stack
{
    char* top;
    size_t capacity;
    size_t size;
    size_t sizeof_element;
};


stack_t* StackCreate(size_t capacity, size_t sizeof_element)
{
	stack_t* stack = (stack_t*)malloc(sizeof(stack_t) + (capacity * sizeof_element));
	
	assert (0 != capacity);
	assert (0 != sizeof_element);
		
	if (NULL == stack)
	{
		return NULL;
	}
	stack->capacity = capacity;
	stack->size = 0;
	stack->sizeof_element = sizeof_element;	
	stack->top = (char*)(stack + sizeof(stack_t));
	
	return stack;
}


void StackDestroy(stack_t* stack)
{
	assert(NULL != stack);
	
/*	stack->top = (stack->top - (stack->size * (stack->sizeof_element)));*/
/*	free(stack->top);*/
	free(stack);
}


void* StackPop(stack_t* stack)
{
	assert(NULL != stack);
	
	stack->top = (stack->top - stack->sizeof_element);
	--stack->size;
	return stack->top;
}


void StackPush(stack_t* stack, void* item)
{
	assert(NULL != stack);
	assert(NULL != item);
	
	memcpy(stack->top, item, stack->sizeof_element);
	stack->top += stack->sizeof_element;
	++stack->size;
}


void* StackPeek(const stack_t* stack)
{
	assert(NULL != stack);
	
	return	(stack->top - stack->sizeof_element);
}


int StackIsEmpty(const stack_t* stack)
{
	assert(NULL != stack);
	
	return (0 == stack->size);
}


size_t StackSize(const stack_t* stack)
{
	assert(NULL != stack);
	
	return stack->size;
}


size_t StackCapacity(const stack_t* stack)
{
	assert(NULL != stack);
	
	return stack->capacity;
}






