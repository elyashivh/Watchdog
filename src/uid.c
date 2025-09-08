/*	Author: Elyashiv Horden
	Date:	22-12-24
	CR: 	Maor Sragovich*/

#include <time.h>		/*time*/
#include <stddef.h>		/*sie_t*/
#include <string.h>		/*strcmp*/
#include <unistd.h>		/*getpid*/
#include <arpa/inet.h> 	/*inet_ntoa*/
#include <ifaddrs.h> 	/*getifaddrs & structs*/
#include <assert.h>		/*assert*/


#include "uid.h"

#define IP_SIZE (14)

typedef enum
{
	OPERATION_FAIL = -1,
	SUCCESS
} status_t;

typedef enum
{
	FALSE,
	TRUE
} bool_t;


const UID_t BadUID = {0};


static int SetTime(UID_t* uid);
static void SetPid(UID_t* uid);
static int SetIP(UID_t* res);

UID_t UIDCreate(void)
{
	UID_t ans = {0};
	static size_t counter = 0;	
	
	if(OPERATION_FAIL == SetIP(&ans))
	{
		return BadUID;
	}
	
	if(OPERATION_FAIL == SetTime(&ans))
	{
		return BadUID;
	}
	
	SetPid(&ans);
	ans.counter = counter;
	++counter;
	
	return ans;
}


int UIDIsEqual(UID_t uid1, UID_t uid2)
{
	if (uid1.time != uid2.time) /*check timediff*/
	{
		return FALSE;
	}
	if (uid1.counter != uid2.counter)
	{
		return FALSE;
	}
	if (uid1.pid != uid2.pid)
	{
		return FALSE;
	}
	if (strncmp((char*)uid1.ip, (char*)uid2.ip, IP_SIZE))
	{
		return FALSE;
	}
	
	return TRUE;
}

static int SetTime(UID_t* uid)
{
	uid->time = time(NULL);
	
	if (OPERATION_FAIL == uid->time)
	{
		return OPERATION_FAIL;
	}
	
	return SUCCESS;
}

static void SetPid(UID_t* uid)
{
	uid->pid = getpid();
}


static int SetIP(UID_t* uid)
{
	struct ifaddrs* ifap = NULL;
	struct ifaddrs* ifa = NULL;
    struct sockaddr_in* sa = NULL;
    char *addr = NULL;

    if(OPERATION_FAIL == getifaddrs(&ifap))
    {
    	return OPERATION_FAIL;
    }
    
    for (ifa = ifap; NULL != ifa; ifa = ifa->ifa_next)
    {
        if (ifa->ifa_addr != NULL && ifa->ifa_addr->sa_family == AF_INET && 0 != strcmp(ifa->ifa_name,"lo")) 
        {
            sa = (struct sockaddr_in*) ifa->ifa_addr;
            addr = inet_ntoa(sa->sin_addr);
            strncpy((char*)uid->ip,addr,IP_SIZE);
            
            freeifaddrs(ifap);
    		return SUCCESS;
        }
    }

    freeifaddrs(ifap);
    return OPERATION_FAIL;
}






