#ifndef __TASK_H__
#define __TASK_H__ 

#include "uid.h"

typedef int(*operation_t)(void* args);
typedef void(*cleanup_op_t)(void* cleanup_args); /* new */


typedef struct task
{
    UID_t id;
    time_t time_to_run;
    size_t interval;
    operation_t operation;
    void* args;
    cleanup_op_t cleanup_op; /* new */ /* can be NULL */
    void* cleanup_args;/* new */
} task_t;


/*
    Description:  
    Args: 
    Return Value: 
    Time Complexity: O()
    Space Complexity: O()
*/
task_t* TaskCreate(operation_t operation, void* args, size_t interval,
                     cleanup_op_t cleanup_op, void* cleanup_args);/* new */


/*
    Description:  
    Args: 
    Return Value: 
    Time Complexity: O()
    Space Complexity: O()
*/
void TaskDestroy(task_t* task); /* should run cleanup_op */


/*
    Description:  
    Args: 
    Return Value: 
    Time Complexity: O()
    Space Complexity: O()
*/
int TaskRun(task_t* task);


/*
    Description:  
    Args: 
    Return Value: 
    Time Complexity: O()
    Space Complexity: O()
*/
time_t TaskGetTimeToRun(const task_t* task);


/*
    Description:  
    Args: 
    Return Value: 
    Time Complexity: O()
    Space Complexity: O()
*/
UID_t TaskGetUID(const task_t* task);


/*
    Description:  
    Args: 
    Return Value: 
    Time Complexity: O()
    Space Complexity: O()
*/
int TaskIsMatch(const task_t* task1, const task_t* task2);


/*
    Description:  
    Args: 
    Return Value: 
    Time Complexity: O()
    Space Complexity: O()
*/
void TaskUpdateTimeToRun(task_t* task);

#endif /* end of header guard  */
