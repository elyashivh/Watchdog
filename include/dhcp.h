/*
    Version 1.2.0
*/

#ifndef __DHCP_H__
#define __DHCP_H__

#include <stddef.h>

typedef enum
{
    SUCCESS = 0,
    MALLOC_FAIL,
    NO_IP_IN_RANGE,
    PREFERRED_IP_TAKEN,
    INVALID_SUBNET,
    REMOVE_FAIL
} dhcp_status_t;

typedef struct dhcp dhcp_t;
/*
    Description:        The function allocates DHCP, initializes Network address, Server address, Broadcast address,
                        according to subnet_bits mask.
                        The DHCP will be allocated according to the subnet IP and the subnet mask bits.
    Args:               subnet_IP - the subnet IP for configuration.
                        subnet_bits - the bits for subnet, defines the host and net.
    Return value:       Pointer to DHCP on success, NULL otherwise.
    Time complexity:    O(log(n))
    Space complexity:   O(1)
*/
dhcp_t* DHCPCreate(const char* subnet_IP, int subnet_bits); 

/*
    Description:        Allocate new IP for the user.
                        This function should receive a preferred IP as format X.X.X.X
                        X consists of a most 3 digits, with no leading zero.
                        i.e : 200.10.1.0 is valid, 200.10.01.0 is invalid.
                        If there's no preferred Ip format of 0.0.0.0 should be sent.
                        Allocate in full capacity is undefined. (refer DHCPCountFree)
    Args:               Pointer to DHCP DS.
                        Preferred_IP - string represents the desired IP (refer DHCPCreate).
    Return value:       SUCCESS - case preferred_IP is available.
                        FAILURE - case none IP was available.
                        PREFERRED_IP_TAKEN - case allocation succeed, but the preferred ip was taken,
                        it is guaranteed to be returned the smallest ip available, though higher than preferred.
                        INVALID_SUBNET - case subnet sent wasn't valid. (Refer DHCPCreate)
    Time complexity:    O(1)
    Space complexity:   O(1) 
*/
dhcp_status_t DHCPAllocateIP(dhcp_t* dhcp, char* preferred_IP);
/*
    Description:        Frees an allocated IP. Case IP is not allocated - nothing happens.
    Args:               Pointer to DHCP DS.
                        IP - string represents an IP (refer DHCPCreate).
    Return value:       SUCCESS case an IP was freed successfully, FAILURE - otherwise.
    Time complexity:    O(1).
    Space complexity:   O(1).
*/
dhcp_status_t DHCPFreeIP(dhcp_t* dhcp, const char* IP);
/*
    DescrIPtion:        Counts the number of IP addresses available in the DHCP.     
    Args:               Pointer to DHCP DS.
    Return value:       The number of available IP addresses.
    Time complexity:    O(n)    
    Space complexity:   O(1)
*/
size_t DHCPCountFree(const dhcp_t* dhcp);
/*
    DescrIPtion:        Destroys the DHCP DS and free all allocted memory.
    Args:               Pointer to DHCP DS.
    Return value:       None.
    Time complexity:    O(n).
    Space complexity:   O(1).
*/
void DHCPDestroy(dhcp_t* dhcp);



#endif /* __DHCP_H__ */