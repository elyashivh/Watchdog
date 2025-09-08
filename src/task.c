#include <stdlib.h>		/*malloc & free*/
#include <time.h>		/*time*/
#include <assert.h>		/*assert*/

#include "task.h"

#define TRUE (1)
#define FALSE (0)
#define SUCCESS (0)
#define FAILURE (-1)


task_t* TaskCreate(operation_t operation, void* args, size_t interval,
					 cleanup_op_t cleanup_op, void* cleanup_args)/* new */
{
	task_t* task = NULL;
	
	assert(operation);
	
	task = (task_t*)malloc(sizeof(task_t));
	if (!task)
	{
		return NULL;
	}
	
	task->id = UIDCreate();
	if (UIDIsEqual(task->id, BadUID))
	{
		free(task);
		return NULL;
	}
	
	task->operation = operation;
	task->time_to_run = time(NULL);
	if (FAILURE == task->time_to_run)
	{
		free(task);
		return NULL;
	}
	
	task->interval = interval;
	task->time_to_run += interval;
	task->args = args;
	task->cleanup_op = cleanup_op;
	task->cleanup_args = cleanup_args;
	
	return task;
}

void TaskDestroy(task_t* task) /* should run cleanup_op */
{
	assert(task);

	if (task->cleanup_op)
	{
		(task->cleanup_op)(task->cleanup_args);
	}

	free(task);
}

int TaskRun(task_t* task)
{
	int rerun_task = FALSE;
	
	assert(task);
	
	rerun_task = task->operation(task->args);
	
	return rerun_task;
}

time_t TaskGetTimeToRun(const task_t* task)
{
	assert(task);
	
	return task->time_to_run;
}

UID_t TaskGetUID(const task_t* task)
{
	assert(task);
	
	return task->id;
}

int TaskIsMatch(const task_t* task1, const task_t* task2)
{
	assert(task1);
	assert(task2);	
	
	if (!UIDIsEqual(task1->id, task2->id))
	{
		return FALSE;
	}
	
	return TRUE;
}

void TaskUpdateTimeToRun(task_t* task)/*return void*/
{
	assert(task);
	
	task->time_to_run = time(NULL);
	if (FAILURE == task->time_to_run)
	{
		free(task);
		return;
	}
	
	task->time_to_run += task->interval;	
}



