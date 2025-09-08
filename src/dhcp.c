/*
Author:     Elyashiv Horden  
Date:		23-1-2024	    
CR: 	    Matan Tabachink
*/

#include <assert.h>     /*assert*/
#include <stddef.h>     /*size_t*/
#include <stdlib.h>     /*malloc & free*/
#include <aio.h>        /*uint32_t*/
#include <string.h>     /*memcpy*/
#include <math.h>        /*pow*/

#include "dhcp.h"
#include "trie.h"

#define IP_LEN (16)
#define FAIL (-1)
#define NUM_OF_BITS (32)
#define DEFUALT_IP (0)

struct dhcp
{
    trie_node_t* root;
    int subnet_ip;
    int subnet_bits;
};



/*                       FORWARD DECLARATION                          */
/******************************************************************** */
static int DHCPInit(dhcp_t* dhcp);
static u_int32_t IpToUInt(const char* subnet_ip);
static void UintToIp(u_int32_t ret_ip, char* ip);
static int CpyOctet(char* ip, u_int32_t num);
static u_int32_t SetBit(u_int32_t num, size_t index);
static char* Itoa(char* dst, u_int32_t num);
static size_t CountDigits(int num);
/********************************************************************* */


/*                           API FUNCTION                               */
/********************************************************************** */
dhcp_t* DHCPCreate(const char* subnet_ip, int subnet_bits)
{
    u_int32_t ip_to_int = 0;
    dhcp_t* dhcp = (dhcp_t*)malloc(sizeof(dhcp_t));
    if (NULL == dhcp)
    {
        return NULL;
    }

    ip_to_int = IpToUInt(subnet_ip);
    if (FAIL == (int)ip_to_int)
    {
        free (dhcp);
        return NULL;
    }
    assert(subnet_ip);
    assert(subnet_bits > 0 && subnet_bits <= 30);

    dhcp->subnet_bits = subnet_bits;
    dhcp->subnet_ip = ip_to_int >> (NUM_OF_BITS-subnet_bits);
    dhcp->root = TrieCreate(dhcp->root);
    if (NULL == dhcp->root)
    {
        free (dhcp);
        return NULL;
    }

    if (SUCCESS != DHCPInit(dhcp))
    {
        TrieFreeNode(dhcp->root);
        free(dhcp);
        return NULL;
    }
    return dhcp;
}

dhcp_status_t DHCPAllocateIP(dhcp_t* dhcp, char* preferred_ip)
{
    u_int32_t pref_ip = 0;
    u_int32_t ret_ip = 0;
    u_int32_t subnet = 0;
    dhcp_status_t status = SUCCESS;

    assert(dhcp);
    assert(preferred_ip);
    

    pref_ip = (u_int32_t)IpToUInt((const char*)preferred_ip);
    if (FAIL == (int)pref_ip)
    {
        return INVALID_SUBNET;
    }
    
    ret_ip = pref_ip;
    subnet = (pref_ip >> (NUM_OF_BITS - dhcp->subnet_bits));

    if (subnet != (u_int32_t)dhcp->subnet_ip && pref_ip != 0)
    {
        return INVALID_SUBNET;
    }
    
    if (DEFUALT_IP == pref_ip)
    {
        pref_ip = ((dhcp->subnet_ip) << (NUM_OF_BITS-dhcp->subnet_bits)) + 1;
    }

    status = TrieInsert(dhcp->root, &pref_ip, NUM_OF_BITS-dhcp->subnet_bits-1);
    if (MALLOC_FAIL == status || NO_IP_IN_RANGE == status)
    {
        return status;
    }
    if (ret_ip != pref_ip)
    {
        status = PREFERRED_IP_TAKEN;
    }
    
    UintToIp(pref_ip, preferred_ip);

    return status;
}

dhcp_status_t DHCPFreeIP(dhcp_t* dhcp, const char* ip)
{
    u_int32_t ip_to_free = 0;
    u_int32_t subnet_bits = 0;
    u_int32_t host_bits = 0;
    dhcp_status_t status = SUCCESS;

    assert(dhcp);
    assert(ip);

    ip_to_free = IpToUInt(ip);
    if (FAIL == (int)ip_to_free)
    {
        return REMOVE_FAIL;
    }

    subnet_bits = ip_to_free;
    subnet_bits = subnet_bits >> (NUM_OF_BITS - dhcp->subnet_bits);
    assert((int)subnet_bits == dhcp->subnet_ip);

    host_bits = ip_to_free << dhcp->subnet_bits;
    host_bits = host_bits >> dhcp->subnet_bits;

    TrieRemove(dhcp->root, host_bits, NUM_OF_BITS - dhcp->subnet_bits-1, &status);
    
    return status;
}

size_t DHCPCountFree(const dhcp_t* dhcp)
{
    assert(dhcp);

    return pow(2, NUM_OF_BITS - dhcp->subnet_bits) - TrieLeafsCounter(dhcp->root);
}

void DHCPDestroy(dhcp_t* dhcp)
{
    TrieFreeNode(dhcp->root);
    free(dhcp);
}



/*                         STATIC FUNCTION                             */
/********************************************************************** */
static int DHCPInit(dhcp_t* dhcp)
{
    dhcp_status_t status = SUCCESS;
    char ip_str[IP_LEN] = {0};
    u_int32_t ip = 0;
    size_t i = 0;

    ip = dhcp->subnet_ip;
    ip = ip << (NUM_OF_BITS-dhcp->subnet_bits);

    
    /*Allocate Network Address*/
    UintToIp(ip, ip_str);
    status = DHCPAllocateIP(dhcp, ip_str);
    if (SUCCESS!= status)
    {
        return status;
    }

    /*Allocate Broadcast Address*/
    for (i = 0; (int)i < (NUM_OF_BITS-dhcp->subnet_bits); ++i)
    {
        ip = SetBit(ip, i);
    }
    UintToIp(ip, ip_str);
    status = DHCPAllocateIP(dhcp, ip_str);
    if (SUCCESS!= status)
    {
        return status;
    }

    /*Allocate Server Address */
    --ip;
    UintToIp(ip, ip_str);
    status = DHCPAllocateIP(dhcp, ip_str);
    if (SUCCESS!= status)
    {
        return status;
    }

    return status;
}

static u_int32_t IpToUInt(const char* subnet_ip)
{
    u_int32_t retval = 0;
    const char *p = subnet_ip;
    char *endp = NULL;
    
    u_int32_t first_octet = 0;
    u_int32_t sec_octet = 0;
    u_int32_t third_octet = 0;
    u_int32_t fourth_octet = 0;

    first_octet = strtoul(p, &endp, 10);
    if (*endp != '.')
    {
        return FAIL;
    } 
    sec_octet = strtoul(p = endp+1, &endp, 10);
    if (*endp != '.')
    {
        return FAIL;
    }
    third_octet = strtoul(p = endp+1, &endp, 10);
    if (*endp != '.')
    {
        return FAIL;
    } 
    fourth_octet = strtoul(p = endp+1, &endp, 10);
    if (*endp)
    {
        return FAIL;
    }
    
    retval = (u_int32_t)((first_octet << 24) | (sec_octet << 16) | (third_octet << 8) | fourth_octet);

    return retval;
}

static void UintToIp(u_int32_t ret_ip, char* ip)
{
    char temp[IP_LEN+1] = {0};
    u_int32_t first_octet = ret_ip >> 24;
    u_int32_t sec_octet = ret_ip;
    u_int32_t third_octet = ret_ip;
    u_int32_t fourth_octet = ret_ip;
    int i = 0;

    memcpy(temp, ip, IP_LEN);

    sec_octet = sec_octet << 8;
    sec_octet = sec_octet >> 24;
    third_octet = third_octet << 16;
    third_octet = third_octet >> 24;
    fourth_octet = fourth_octet << 24;
    fourth_octet = fourth_octet >> 24;

    i += CpyOctet(&temp[i], first_octet);
    i += CpyOctet(&temp[i], sec_octet);
    i += CpyOctet(&temp[i], third_octet);
    CpyOctet(&temp[i], fourth_octet);

    temp[(int)strlen(temp)-1] = '\0';
    temp[IP_LEN-1] = '\0';
    memcpy(ip, temp, IP_LEN);
}

static int CpyOctet(char* ip, u_int32_t num)
{
    int i = CountDigits(num);
    Itoa(ip, num);
    ip = ip + i;
    memcpy(ip, ".", sizeof(char));
    return i+1;
}

static u_int32_t SetBit(u_int32_t num, size_t index)
{
	size_t mask = (size_t)1;
	
	mask <<= index;
	
	return mask | num;
}

static char* Itoa(char* dst, u_int32_t num)
{
	u_int32_t char_count = CountDigits(num);
		
	dst[char_count] = '\0';
	char_count--;
	
	if (0 == num)
	{
		dst[char_count] = '0';
		return dst;
	}
	
	while ( 0 < num )
	{
		dst[char_count] = '0' + (num % 10);
		char_count--;
		num = num / 10;
	}

	return dst;
}

static size_t CountDigits(int num) 
{ 
	int count = 0; 
    if (0 == num)
    {
        return 1;
    }
	for (count = 0; num > 0; num /= 10, count++); 
	return count; 
} 
 

