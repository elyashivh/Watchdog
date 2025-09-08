/*
Author:     Elyashiv Horden
Date:		9-2-2025
CR:         Yehonatan Ailon
*/
#define _GNU_SOURCE
#include <assert.h>    /*assert*/
#include <unistd.h>    /*pit_t*/
#include <stdlib.h>    /*malloc, free*/
#include <stdio.h>     /*sprintf*/
#include <pthread.h>   /*threads*/
#include <semaphore.h> /*semaphore*/
#include <signal.h>     /*sigaction*/

#include "wd.h"
#include "scheduler.h"
#include "uid.h"
#include "utils_wd.h"

#define COLOR_MAGENTA "\x1b[35m"
#define RESET_ALL "\x1b[0m"

#define CHILD_PROCESS (0)
#define ERROR (-1)
#define RESET (0)
#define TRUE (1)
#define RERUN (1)
#define FALSE (0)
#define FILE_NAME_SIZE (20)
#define PID_KEY ("pid")
#define WD_FILE_NAME "./wd_process.out"
#define WD_SEM_NAME ("/wd")
#define USER_SEM_NAME ("/user")


/*                            GLOBAL VARS                             */
/******************************************************************** */
wd_args_t user_args;
pthread_t user_threads = {0};
scheduler_t *user_sched;
wd_status_t wd_created_g;

extern sig_atomic_t to_revival_g;
extern sig_atomic_t signal_counter_g;
extern sem_t* wd_sem;
extern sem_t* thread_sem;

/*                       FORWARD DECLARATION                          */
/******************************************************************** */
static wd_status_t CreateWd(int argc, char **argv, size_t interval, int tolerance);
static wd_status_t CreateThread(scheduler_t*);
static void* RunUserThread(void *args);
int CheckSignal(void *args);
int SendSignal(void *args);
static void SetPidToEnv(pid_t pid);



/*                           API FUNCTION                               */
/********************************************************************** */
wd_status_t WDStart(int argc, const char *argv[], size_t interval, int tolerance)
{
    pid_t pid = 0;
    wd_status_t status = WD_SUCCESS;

    assert(argv);
    InitArgs(&user_args, argc,(char**)argv, interval, tolerance);

    UnLinkSem();
    
    if (WD_SUCCESS != OpenSem())
    {
        UnLinkSem();
        return WD_FAIL;
    }
    
    user_sched = InitScheduler(user_sched, SendSignal, CheckSignal, &user_args);
    if (NULL == user_sched)
    {
        UnLinkSem();
        return WD_FAIL;
    }

    status = InitHandlers();
    if (WD_SUCCESS != status)
    {
        UnLinkSem();
        SchedulerDestroy(user_sched);
        return WD_FAIL;
    }
    to_revival_g = TRUE;

    pid = fork();
    if (CHILD_PROCESS == pid)
    {
        if (WD_SUCCESS != CreateWd(argc, (char**)argv, interval, tolerance))
        {
            SchedulerDestroy(user_sched);
            return WD_FAIL;
        }
    }
    else if (CHILD_PROCESS < pid)
    {
        sem_post(thread_sem);
        sem_wait(wd_sem);
        SetPidToEnv(pid);
        if (WD_SUCCESS != CreateThread(user_sched))
        {
            return WD_FAIL;
        }
    }

    return WD_SUCCESS;
}

void WDStop()
{
    pid_t pid_to_kill = atoi(getenv(PID_KEY));

    kill(pid_to_kill, SIGUSR2);
    to_revival_g = FALSE;

    SchedulerStop(user_sched);
    CloseSem();
    UnLinkSem();
    
    pthread_join(user_threads, NULL);
    SchedulerDestroy(user_sched);
}

/*                         STATIC FUNCTION                             */
/********************************************************************** */
static wd_status_t CreateWd(int argc, char **argv, size_t interval, int tolerance)
{
    int i = 0;
    char interval_in_str[FILE_NAME_SIZE] = {0};
	char tolerance_in_str[FILE_NAME_SIZE] = {0};

    user_args.argc = argc;
    user_args.interval = interval;
    user_args.tolerance = tolerance;
    
    user_args.argv = malloc((argc+4)*sizeof(char*));
    if (NULL == user_args.argv)
    {
        return WD_FAIL;
    }
    
    sprintf(interval_in_str, "%ld", interval);
    sprintf(tolerance_in_str, "%d", tolerance);
    
    user_args.argv[0] = WD_FILE_NAME;
    user_args.argv[1] = interval_in_str;
    user_args.argv[2] = tolerance_in_str;
    for (i = 0; i < argc; ++i)
    {
        user_args.argv[i+3] = (char*)argv[i];
    } 
    
    user_args.argv[i+3] = NULL;
    if (SUCCESS != execvp(WD_FILE_NAME, user_args.argv))
    {
        return WD_FAIL; 
    }
    
    return WD_SUCCESS;
}

static wd_status_t CreateThread(scheduler_t* user_sched)
{
    if (WD_SUCCESS != pthread_create(&user_threads, NULL, RunUserThread, user_sched))
    {
        return WD_FAIL;
    }
    return WD_SUCCESS;
}

static void* RunUserThread(void *args)
{
    while (SchedulerRun((scheduler_t*)args) ==  STOP && to_revival_g == TRUE)
    {
        /*empty loop*/
    }
    
    return NULL;
}

int SendSignal(void *args)
{
    pid_t send_to_pid = 0;
    char* pid_as_env = NULL;

    pid_as_env = getenv(PID_KEY);
    send_to_pid = atoi(pid_as_env);

    kill(send_to_pid, SIGUSR1);
    write(1, COLOR_MAGENTA"ping\n"RESET_ALL, 10);

    (void)args;
    return RERUN;
}

int CheckSignal(void *args)
{
    pid_t new_pid = 0;
    char pid_as_env[FILE_NAME_SIZE] = {0};

    ++signal_counter_g;

    if (to_revival_g == FALSE)
    {
        SchedulerStop(user_sched);
    }
    
    if (signal_counter_g >= user_args.tolerance)
    {
        SignalHandlerReset();
        new_pid = fork();

        sprintf(pid_as_env, "%d", new_pid);
        setenv(PID_KEY, pid_as_env, 1);
        
        if (CHILD_PROCESS == new_pid)
        {
            CreateWd(user_args.argc, (char**)user_args.argv, user_args.interval,
            user_args.tolerance);
            sem_post(thread_sem);
            sem_wait(wd_sem);
        }
        else
        {
            SchedulerStop(user_sched);
        }
        
    }

    (void)args;

    return RERUN;
}

static void SetPidToEnv(pid_t pid)
{
    char buffer[FILE_NAME_SIZE] = {0};
    sprintf(buffer, "%d", pid);
    setenv("pid", buffer, TRUE);
}