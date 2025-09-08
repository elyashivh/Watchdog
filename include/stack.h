#ifndef __STACK_H__
#define __STACK_H__

#include <stddef.h>

typedef struct stack stack_t;

void StackPush(stack_t* stack, void* item);
/*
Description:Push a item into the stack
Args:	stack - the stack. item - the item to push
Return Value:	void
Time Complexity:	O(1)
Space Complexity:	O(1)
*/

void* StackPop(stack_t* stack);
/*
Description:	Remove the first item and return it
Args:	stack - the stack
Return Value:	The first item
Time Complexity:	O(1)
Space Complexity:	O(1)
*/

void* StackPeek(const stack_t* stack);
/*
Description:	show the first item
Args:	stack - the stack
Return Value:	The first item
Time Complexity:	O(1)
Space Complexity:	O(1)
*/

int StackIsEmpty(const stack_t* stack);
/*
Description:	Check if the stack is empty
Args:	stack - the stack
Return Value:	1 - if true, 0 - otherwise
Time Complexity:	O(1)
Space Complexity:	O(1)
*/

size_t StackSize(const stack_t* stack);
/*
Description:	Check the stack's size
Args:	stack - the stack
Return Value:	stack's size
Time Complexity:	O(1)
Space Complexity:	O(1)
*/
size_t StackCapacity(const stack_t* stack);
/*
Description:	Check the stack's capacity
Args:	stack - the stack
Return Value:	stack's capacity
Time Complexity:	O(1)
Space Complexity:	O(1)
*/

stack_t* StackCreate(size_t capacity, size_t sizeof_element);
/*
Description:	Create a new stack
Args:	capacity - the maximum size of the stack.
		sizeof_element - sizeof stack's items in byts
Return Value:	Return the new stack
Time Complexity:	O(1)
Space Complexity:	O(1)
*/

void StackDestroy(stack_t* stack);
/*
Description:	Delete the stack
Args:	stack - the stack
Return Value:	void
Time Complexity:	O(1)
Space Complexity:	O(1)
*/

#endif /* end of header guard __STACK_H__ */

