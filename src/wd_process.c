

#define _GNU_SOURCE

#include <time.h>
#include <stdlib.h>    /*atoi*/
#include <stdio.h>     /*sprintf*/
#include <semaphore.h> /*semaphore*/
#include <signal.h>    /*sigaction*/

#include "scheduler.h"
#include "utils_wd.h"

#define ANSI_COLOR_CYAN "\x1b[36m"
#define RESET_ALL "\x1b[0m"

#define TRUE (1)
#define RERUN (1)
#define FALSE (0)

/*                            GLOBAL VARS                             */
/******************************************************************** */
wd_args_t wd_args;
scheduler_t *wd_sched;

extern sig_atomic_t to_revival_g;
extern sig_atomic_t signal_counter_g;
extern sem_t *wd_sem;
extern sem_t *thread_sem;
/*                       FORWARD DECLARATION                          */
/******************************************************************** */
static void RunWdProcess(int argc, char **args);
static int CheckSignal(void *args);
static int SendSignal(void *args);

/*                           MAIN FUNCTION                            */
/******************************************************************** */

int main(int argc, char *argv[])
{
    RunWdProcess(argc, argv);

    return 0;
}

/*                         STATIC FUNCTION                             */
/********************************************************************** */
static void RunWdProcess(int argc, char **args)
{
    wd_status_t status = WD_SUCCESS;
    InitArgs(&wd_args, argc, args, (size_t)atoi(args[1]), (int)atoi(args[2]));

    status = OpenSem();
    if (WD_SUCCESS != status)
    {
        kill(getppid(), SIGTERM);
    }
    status = InitHandlers();
    if (WD_SUCCESS != status)
    {
        CloseSem();
        UnLinkSem();
        kill(getppid(), SIGTERM);
    }

    wd_sched = InitScheduler(wd_sched, SendSignal, CheckSignal, &wd_args);
    if (NULL == wd_sched)
    {
        CloseSem();
        UnLinkSem();
        kill(getppid(), SIGTERM);
    }

    to_revival_g = TRUE;
    sem_post(wd_sem);
    sem_wait(thread_sem);
    SchedulerRun(wd_sched);

    SchedulerDestroy(wd_sched);
    UnLinkSem();
    (void)argc;
}

int SendSignal(void *args)
{
    kill(getppid(), SIGUSR1);
    write(1, ANSI_COLOR_CYAN "pong\n" RESET_ALL, 10);

    (void)args;
    return RERUN;
}

int CheckSignal(void *args)
{
    ++signal_counter_g;

    if (to_revival_g == FALSE)
    {
        SchedulerStop(wd_sched);
    }
    else if (signal_counter_g >= wd_args.tolerance)
    {
        if (-1 == execvp(wd_args.argv[3], &wd_args.argv[3]))
        {
            SchedulerStop(wd_sched);
            CloseSem();
            UnLinkSem();
            SchedulerDestroy(wd_sched);
        }
    }
    (void)args;
    return RERUN;
}
