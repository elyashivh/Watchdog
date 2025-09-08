#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__ 

#include <stddef.h> /* include size_t */
#include "uid.h"
#include "pqueue.h"



typedef enum
{
	STOP = -3,
	ENQUEUE_FAIL,
	TIME_FAILURE,
	SUCCESSFULL_RUN
} run_status_t;

typedef struct scheduler scheduler_t;
typedef int (*s_operation_t)(void* args);
typedef void(*s_cleanup_op_t)(void* cleanup_args); /* new */


/*
	Description: Initialize a new Scheduler
	Args: void
	Return Value: scheduler_t* - pointer to the scheduler 
	Time Complexity: O(1)
	Space Complexity: O(1)
*/
scheduler_t* SchedulerCreate(void);


/*
	Description: remove and delete the scheduler
	Args: scheduler_t* pointer to the scheduler
	Return Value: void
	Time Complexity: O(n)
	Space Complexity: O(1)
*/
void SchedulerDestroy(scheduler_t* scheduler);


/*
	Description: insert a new task to the scheduler
	Args: scheduler_t* - pointer to the scheduler
		s_operation_t - operation to be execute,
		void* - arguments fot the operation
		size_t - how many time to execute
		s_cleanup_op_t - function to close tasks
		void* - aruments fot the function
	Return Value: UID_t - the new task id
	Time Complexity: O(n)
	Space Complexity: O(1)
*/
UID_t SchedulerAddTask(scheduler_t* scheduler,s_operation_t operation, void* args,
						 size_t interval, s_cleanup_op_t cleanup_op, void* cleanup_args); /* new*/


/*
	Description: Remove specific task from the scheduler
	Args: scheduler_t* - pointer to the scheduler
		 UID_t - task id to remove
	Return Value: void
	Time Complexity: O(n)
	Space Complexity: O(1)
*/
void SchedulerRemove(scheduler_t* scheduler, UID_t task);


/*
	Description: Remove all tasks from the scheduler
	Args: scheduler_t* - pointer to the scheduler
	Return Value: void
	Time Complexity: O(n)
	Space Complexity: O(1)
*/
void SchedulerClear(scheduler_t* scheduler);


/*
	Description: activate the scheduler
	Args: scheduler_t* - pointer to the scheduler
	Return Value: run_status_t - exit status
	Time Complexity: O(n)
	Space Complexity: O(1)
*/
run_status_t SchedulerRun(scheduler_t* scheduler);


/*
	Description: stop the scheduler running
	Args: scheduler_t* - pointer to the scheduler
	Return Value: void
	Time Complexity: O(1)
	Space Complexity: O(1)
*/
void SchedulerStop(scheduler_t* scheduler);


/*
	Description: check if the scheduler is empty
	Args: scheduler_t* - pointer to the scheduler
	Return Value: 1 - if empty, 0 - otherwise
	Time Complexity: O(1)
	Space Complexity: O(1)
*/
int SchedulerIsEmpty(scheduler_t* scheduler);


/*
	Description: check scheduler size
	Args: scheduler_t* - pointer to the scheduler
	Return Value: size_t represent the num of tasks in the scheduler
	Time Complexity: O(n)
	Space Complexity: O(1)
*/
size_t SchedulerSize(scheduler_t* scheduler);

#endif /* end of header guard __SCHEDULER_H__ */

