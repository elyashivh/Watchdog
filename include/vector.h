#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <stddef.h> /* size_t */

typedef struct vector vector_t;

typedef enum
{
	ALLOCATION_FAIL = -2,
	RESIZE_FAILED,
	SUCCESS

}status_t;

/*
Description: 		Alloc ds of Vector in memory. This ds could dynamically 
					grow when using functions that increases its length.
Args:				size_t - size of each element in the vector.
Return Value:		pointer to the alloced ds.
Time Complexity:	O(1)
Space Complexity:	O(size_of_item)
*/
vector_t* VectorCreate(size_t size_of_item);

/*
Description:		Remove all alloc memory of the ds.
Args:				pointer to the struct.
Return Value:		None.
Time Complexity:	O(1)
Space Complexity:	O(1)
*/
void VectorDestroy(vector_t* vector);

/*
Description:		Pushes another item to the ds. Could cause for
					extension of the array.
Args:				Pointer to struct, element desired to be pushed.
Return Value:		0 in SUCCESS, <0 otherwise.
Time Complexity:	O(1)
Space Complexity:	O(1)
*/
status_t VectorPushBack(vector_t* vector, void* item);

/*
Description:		Reduce the size of vector by one.
					Popping could lead shrinking the vector's capacity.
					Poping from an empty vector is undefined.
Args:				Pointer to struct.
Return Value:		Pointer to the poped element.
Time Complexity:	O(1).
Space Complexity:	O(1).
*/
void VectorPopBack(vector_t* vector);


/*
Description:		Reduce size index of available cell of the vector.
Args:				Pointer to struct.
Return Value:		size_t - index.
Time Complexity:	O(1).
Space Complexity:	O(1).
*/
size_t VectorGetSize(const vector_t* vector);

/*
Description:		Retrieve the size of the vector.
Args:				Pointer to struct.
Return Value:		size_t - capacity.
Time Complexity:	O(1).
Space Complexity:	O(1).
*/
size_t VectorGetCapacity(const vector_t* vector);


/*
Description:		Retrieve pointer for the element using index.
					Accessing vector out of bound is undefined.
Args:				Pointer to struct, index for the vector.
Return Value:		Pointer to the desired element.
Time Complexity:	O(1)
Space Complexity:	O(1)
*/
void* VectorGetAccess(vector_t* vector, size_t index);

/*
Description:		Set new capacity for the vector.
					Case new capacity lower equal to the current capacity
					- Nothing would changed.
Args:				Pointer to struct, new capacity size.
Return Value:		0 on SUCCESS, <0 otherwise.
Time Complexity:	O(size).
Space Complexity:	O(new_capacity).
*/
status_t VectorReserve(vector_t* vector, size_t capacity);

/*
Description:		Shrink the vector by growth_factor.
					Case capacity is set to 1
					- Nothing would changed.
Args:				Pointer to struct.
Return Value:		0 on SUCCESS, <0 otherwise.
Time Complexity:	O(1).
Space Complexity:	O(1).
*/
status_t VectorShrink(vector_t* vector);

#endif /* end of header guard __VECTOR_H__ */
