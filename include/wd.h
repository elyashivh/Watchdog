
/*
    Version 3.0.0
*/

#ifndef __WD_H__
#define __WD_H__

#include <sys/types.h>
#include <time.h>
#include "utils_wd.h"

/* typedef enum wd_status
{
    WD_SUCCESS = 0,
    WD_ALLOCATION_FAIL
} wd_status_t; */

wd_status_t WDStart(int argc, const char* argv[], size_t interval, int tolerance);

void WDStop();

#endif /*__WD_H__*/

