/*	Author: Elyashiv Horden
	Date:	23-12-24
	CR: 	Roi Sasson*/

#include <assert.h>		/*assert*/
#include <stdlib.h>		/*mallo & free*/
#include <time.h>		/*time*/
#include <unistd.h>		/*sleep*/

#include "scheduler.h"
#include "pqueue.h"
#include "task.h"


#define TRUE (1)
#define FALSE (0)
#define OPERATION_FAIL (-1)

struct scheduler
{
    pqueue_t* pqueue;
    int scheduler_is_run;
    int task_is_run;
};

/*                       FORWARD DECLARATION                          */
/******************************************************************** */
static int Comparator(void* task1, void* task2);
static int IsMatchTaskByID(void* data, void* id);
/******************************************************************** */

/*                           API FUNCTION                               */
/********************************************************************** */
scheduler_t* SchedulerCreate(void)
{
	scheduler_t* scheduler = NULL;
	scheduler = (scheduler_t*)malloc(sizeof(scheduler_t));
	if (!scheduler)
	{
		return NULL;
	}
	
	scheduler->pqueue = PQCreate((pq_comperator_t)Comparator);
	if (!scheduler->pqueue)
	{
		free(scheduler);
		return NULL;
	}
	scheduler->scheduler_is_run = FALSE;
	scheduler->task_is_run = FALSE;
	
	return scheduler;
}

void SchedulerDestroy(scheduler_t* scheduler)
{
	assert(scheduler);
	
	SchedulerClear(scheduler);
	PQDestroy(scheduler->pqueue);
	free(scheduler);
}

UID_t SchedulerAddTask(scheduler_t* scheduler,s_operation_t operation, void* args,
		 size_t interval, s_cleanup_op_t cleanup_op, void* cleanup_args) 
{
	task_t* new_task = {0};
	assert(scheduler);
	
	new_task = TaskCreate(operation, args, interval, cleanup_op, cleanup_args);
	if (!new_task)
	{
		return BadUID;
	}
	
	if (OPERATION_FAIL == PQEnqueue(scheduler->pqueue, new_task))
	{
		TaskDestroy(new_task);
		return BadUID;
	}
	
	return TaskGetUID(new_task);
}

void SchedulerRemove(scheduler_t* scheduler, UID_t uid)
{
	task_t* temp = NULL;
	task_t find_task = {0};
	assert(scheduler);
	/*add assert baduid*/
	
	find_task.id = uid;
	temp = PQErase(scheduler->pqueue, &find_task, IsMatchTaskByID);/*use taskismatch*/
	if (NULL != temp)
	{
		/* If get task that already removed */
		TaskDestroy((task_t*)temp);
	}
}

void SchedulerClear(scheduler_t* scheduler)
{
	task_t* temp = NULL;
	assert(scheduler);

	scheduler->task_is_run = FALSE;
	while (!SchedulerIsEmpty(scheduler))
	{
		temp = (task_t*)PQDequeue(scheduler->pqueue);
		TaskDestroy(temp);
	}
}


run_status_t SchedulerRun(scheduler_t* scheduler)
{
	task_t* next_task = {0};
	int to_rerun_task = FALSE;
	time_t time_to_sleep = 0;
	
	assert(scheduler);
	
	scheduler-> scheduler_is_run = TRUE;
	while (!SchedulerIsEmpty(scheduler) && TRUE == scheduler->scheduler_is_run)
	{
		/*get first task to execute*/
		next_task = (task_t*)PQPeek(scheduler->pqueue);
		time_to_sleep = difftime(TaskGetTimeToRun(next_task), time(NULL));

		/*wait for task to execute*/
		while (0 < time_to_sleep && sleep(time_to_sleep))
		{
			/*Empty loop*/
		}

		PQDequeue(scheduler->pqueue);
		/*check if task is repete*/
		scheduler->task_is_run = TRUE;
		to_rerun_task = TaskRun(next_task);
		if (TRUE == to_rerun_task && TRUE == scheduler->task_is_run)
		{
			TaskUpdateTimeToRun(next_task);
			if (OPERATION_FAIL == PQEnqueue(scheduler->pqueue, next_task))
			{
				return ENQUEUE_FAIL;
			}
		}

		/*destroy task*/
		else 
		{
			TaskDestroy(next_task);
		}
		scheduler->task_is_run = FALSE;

	}

	/*check if task == stop*/
	if (FALSE == scheduler->scheduler_is_run)
	{
		return STOP;
	}
	scheduler->scheduler_is_run = FALSE;
	return SUCCESSFULL_RUN;
}

void SchedulerStop(scheduler_t* scheduler)
{
	assert(scheduler);
	
	scheduler->scheduler_is_run = FALSE;
}

int SchedulerIsEmpty(scheduler_t* scheduler)
{
	assert(scheduler);
	if (TRUE == scheduler->task_is_run)
	{
		return FALSE;
	}
	return PQIsEmpty(scheduler->pqueue) ;
}

size_t SchedulerSize(scheduler_t* scheduler)
{
	assert(scheduler);

	return PQSize(scheduler->pqueue) + scheduler->task_is_run;
}


static int Comparator(void* task1, void* task2)
{
	return difftime(TaskGetTimeToRun(task1), TaskGetTimeToRun(task2));
}


static int IsMatchTaskByID(void* task1, void* task2)
{
	return UIDIsEqual(((task_t*)task1)->id, ((task_t*)task2)->id);
}
