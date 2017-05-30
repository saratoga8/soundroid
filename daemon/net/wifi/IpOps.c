/**
 * @file
 * Operations on a local IP or a connected IP 
 *
 **
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Daniel Haimov
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "IpOps.h"
#include "Log.h"
#include "SocketsLib.h"

#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <errno.h>

#include <sys/socket.h> 
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <ifaddrs.h>


#define ETH  "eth" 
#define WLAN "wlan"

#define TAG "IP_OPS"      /**< tag for writing to the log file */

#define UNKNOWN "UNKNOWN"

/**
 * Copy the local IP to the given string
 * @param str String for copying IP to
 */
void copyLocalIp2Str(char *str)
{
    struct ifaddrs *ifaddr, *ifa;
    
    if (getifaddrs(&ifaddr) == ERR) {
	writeToLog2("ERROR in copyLocalIp2Str(): ", strerror(errno), TAG);
	strcpy(str, UNKNOWN);
	return;
    }

    int family, s;
    char host[NI_MAXHOST] = {'\0'};
    size_t sock_addr_size;
    
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
	{
	    if (ifa->ifa_addr == NULL)
		continue;
	    
	    family = ifa->ifa_addr->sa_family;
	    if( (strstr(ifa->ifa_name, ETH) != NULL) || (strstr(ifa->ifa_name, WLAN) != NULL) )
		{
		    if (family == AF_INET || family == AF_INET6)
			{
			    sock_addr_size = (family == AF_INET) ? sizeof(struct sockaddr_in): sizeof(struct sockaddr_in6); 
			    s = getnameinfo(ifa->ifa_addr, sock_addr_size, host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
			    if (s != NO_ERR)
				{
				    writeToLog2("ERROR in copyLocalIp2Str(): ", gai_strerror(s), TAG);
				    break;
				}
			    strcpy(str, host);
			    freeifaddrs(ifaddr);
			    return;
			}
		}
	}
    strcpy(str, UNKNOWN);
    freeifaddrs(ifaddr);
}

/**
 * Copy the IP of the connected client to the given string 
 * @param sockDescr Socket descriptor
 * @param str String for copying IP to
 */
void copyConnectedIp2Str(const int sockDescr, char *str)
{
    struct sockaddr_in name = {0};
    socklen_t namelen = sizeof(name);
    if(getpeername(sockDescr, (struct sockaddr*) &name, &namelen) == ERR)
	{
	    writeToLog2("ERROR setConnectedIP(): ", strerror(errno), TAG);
	    strcpy(str, UNKNOWN);
	    return;
	}
         
    char buffer[INET_ADDRSTRLEN];
    const char *ip = inet_ntop(AF_INET, &name.sin_addr, buffer, INET_ADDRSTRLEN);
    if(ip == NULL)
	{
	    writeToLog2("ERROR setConnectedIP(): ", strerror(errno), TAG);
	    strcpy(str, UNKNOWN);
	    return;	    
	}

    strcpy(str, ip);
}

