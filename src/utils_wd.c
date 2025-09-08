#define _GNU_SOURCE    /*sigaction*/
#include <unistd.h>    /*pit_t*/
#include <stdlib.h>    /*malloc, free*/
#include <stdio.h>     /*sprintf*/
#include <pthread.h>   /*threads*/
#include <semaphore.h> /*semaphore*/
#include <fcntl.h>     /* O_* constants */
#include <sys/stat.h>  /* mode constants */
#include <signal.h>    /*sigaction*/
#include <string.h>    /*strcpy*/

#include "utils_wd.h"
#include "wd.h"
#include "scheduler.h"

#define RESET (0)
#define FALSE (0)
#define WD_FILE_NAME "./wd_process.out"
#define WD_SEM_NAME ("/wd")
#define USER_SEM_NAME ("/user")


sem_t *wd_sem;
sem_t *thread_sem;
sig_atomic_t to_revival_g;
sig_atomic_t signal_counter_g;

void SignalWdHandlerReset()
{
    signal_counter_g = RESET;
}

void SignalHandlerKill()
{
    to_revival_g = FALSE;
}

void SignalHandlerReset()
{
    signal_counter_g = RESET;
}

wd_status_t InitHandlers()
{
    struct sigaction user_action = {0};
    struct sigaction kill_action = {0};

    user_action.sa_handler = SignalHandlerReset;
    kill_action.sa_handler = SignalHandlerKill;

    if (SUCCESS != sigaction(SIGUSR1, &user_action, NULL))
    {
        return WD_FAIL;
    }
    if (SUCCESS != sigaction(SIGUSR2, &kill_action, NULL))
    {
        return WD_FAIL;
    }

    return 0;
}

scheduler_t *InitScheduler(scheduler_t * sched, wd_signal_handler_t send_signal,
                             wd_signal_handler_t check_signal,  wd_args_t* args)
{
    sched = SchedulerCreate();
    if (NULL == sched)
    {
        return NULL;
    }

    if (UIDIsEqual(BadUID, SchedulerAddTask(sched, send_signal, NULL, args->interval, NULL, NULL)))
    {
        SchedulerDestroy(sched);
        return NULL;
    }
    if (UIDIsEqual(BadUID, SchedulerAddTask(sched, check_signal, &(args->tolerance), args->interval, NULL, NULL)))
    {
        SchedulerDestroy(sched);
        return NULL;
    }

    return sched;
}


wd_status_t OpenSem()
{
    wd_sem = sem_open(WD_SEM_NAME, O_CREAT, S_IRWXU, 0);
    thread_sem = sem_open(USER_SEM_NAME, O_CREAT, S_IRWXU, 0);

    if (SEM_FAILED == wd_sem || SEM_FAILED == thread_sem)
    {
        return WD_FAIL;
    }

    return WD_SUCCESS;
}

void InitArgs(wd_args_t* args, int argc, char **argv, size_t interval, int tolerance)
{
    args->argc = argc;
    args->argv = argv;
    args->interval = interval;
    args->tolerance = tolerance;
    strcpy(args->wd_file_name, WD_FILE_NAME);
}

void UnLinkSem()
{
    sem_unlink(WD_SEM_NAME);   
    sem_unlink(USER_SEM_NAME); 
}

void CloseSem()
{
    sem_close(wd_sem);
    sem_close(thread_sem);
}