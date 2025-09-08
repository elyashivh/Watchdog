
/*
    Version 3.0.0
*/

#ifndef __WD_UTILS_H__
#define __WD_UTILS_H__


#include <sys/types.h>
#include <time.h>
#include "scheduler.h"

#define FILE_NAME_SIZE (20)

typedef int (*wd_signal_handler_t)(void* args);

typedef enum wd_status
{
    WD_SUCCESS = 0,
    WD_FAIL
} wd_status_t;

typedef struct
{
    int argc;
    char** argv;
    char wd_file_name[FILE_NAME_SIZE];
    size_t interval;
    int tolerance;
} wd_args_t;

void SignalWdHandlerReset();

void SignalHandlerKill();

wd_status_t InitHandlers();

scheduler_t *InitScheduler(scheduler_t * sched, wd_signal_handler_t send_signal,
    wd_signal_handler_t check_signal,  wd_args_t* args);

void SignalHandlerReset();

wd_status_t OpenSem();

void InitArgs(wd_args_t* args, int argc, char **argv, size_t interval, int tolerance);

void UnLinkSem();

void CloseSem();

#endif /*__WD_UTILS_H__ */

