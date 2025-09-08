#ifndef __UID_H__
#define __UID_H__

#include <stddef.h>		
#include <time.h>		
#include <sys/types.h>

typedef struct UID
{
    time_t time;
    size_t counter;
    pid_t pid;
    unsigned char ip[14];
}UID_t;

extern const UID_t BadUID;

UID_t UIDCreate(void);

int UIDIsEqual(UID_t uid1, UID_t uid2);


#endif /* End of the header guard UID */
