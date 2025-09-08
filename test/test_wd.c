#include <time.h>
#include <stdio.h>

#include "unistd.h"
#include "wd.h"




int main (int argc, char *argv[])
{
    time_t start = 0;
    if(WDStart(argc, (const char**)argv, 1, 5))
    {
        printf("wd failed\n");
        return 0;
    }

    printf("Starting critical section...\n");

    start = time(NULL);
    while (time(NULL) != start + 10);

    printf("Finished critical section!\n");
    WDStop();
    return 0;
}


