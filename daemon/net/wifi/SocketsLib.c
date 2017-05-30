/**
 * @file
 * The library for connecting by sockets
 * \var mutexData
 * \brief The mutex for sent/received data
 * \var mutexStop 
 * \brief The mutex for run status
 * \def DATA_LEN
 * \brief The length of the sent/received data array
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

#include "SocketsLib.h"
#include "synchronise.h"
#include "Log.h"
#include "addr.h"

#include <errno.h>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include <sys/socket.h> 
#include <sys/types.h>
#include <netdb.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>

#include "IpOps.h"

#define MAX_SOCKETS_NUM_WAITED_FOR_ACCEPT 5     /**< The maximal number of sockets waiting for accept */

#define STR_END "end"

#define TAG "SOCKETS_LIB"                       /**< The tag for writing to the log file */

#define IP_ADDR_STR_LEN 20                      /**< The length of the IP string */
char connectedIP[IP_ADDR_STR_LEN] = {'\0'};     /**< The buffer for a connected IP addres string */
char localIP    [IP_ADDR_STR_LEN] = {'\0'};     /**< The buffer for the local IP addres string */

#define PORT_NUM_LEN 10                         /**< The length of the port number string */
char portNum[PORT_NUM_LEN] = {'\0'};            /**< The buffer for the port number string */


fd_set master;    // master file descriptor list
fd_set read_fds;  // temp file descriptor list for select()
int fdmax;        // maximum file descriptor number

/*
void writeToLog2(const char* txt1, const char* txt2)
{
    #define BUFF_LEN 100
    char buff[BUFF_LEN] = {'\0'};
    if(sprintf(buff, txt1, txt2) < 0)
	{
	    printf("ERROR: can't copy the texts '%s", txt1);
	    printf("' and '%s", txt2);
	    printf("' to the buffer with the size %d\n", BUFF_LEN);
	    return;
	}
    writeToLog(buff, TAG);
    }*/

/**
 * Get the string of the last error
 * @return The string of the last error
 */
const char* getErrStr()
{
	return strerror(errno);
}

/**
 * Get local IP
 * @return The string of the local IP or ""
 */
const char* getLocalAddr()
{
    return localIP;
}

/**
 * Get connected IP
 * @return The string of the connected IP or ""
 */
const char* getConnectedAddr()
{
    return connectedIP;
}

/**
 * Set port number
 * @param port The port number string
 * @return ERR or NO_ERR
 */
const int setPort(const char* port)
{
    char buff[50] = {'\0'};
    if(port == NULL)
	{
	    writeToLog("ERROR setPort(): the given pointer to a port number string is NULL\n", TAG);
	    return ERR;
	}
    const size_t len = strlen(port);
    if(len == 0)
	{
	    writeToLog("ERROR setPort(): the given string of a port number is EMPTY\n", TAG);
	    return ERR;
	}
    if(len >= PORT_NUM_LEN)
	{
    	char err[100] = {'\0'};
	    sprintf(err, "ERROR setPort(): can't change the port. The LENGTH of the given port number %s is greater then %d", port, PORT_NUM_LEN);
	    writeToLog(err, TAG);
	    return ERR;
	}

    bzero(buff, strlen(buff));
    sprintf(buff, "\tChanging port from %s to %s\n", portNum, port);
    writeToLog(buff, TAG);

    if(strlen(portNum))
	bzero(portNum, PORT_NUM_LEN);

    strcpy(portNum, port);

    return NO_ERR;
}

/**
 * Get the currently used port
 * @return the string of the currently used port number
 */
char* getPort() 
{ 
  return portNum; 
}

/**
 * Check wether the given string is number
 * @param str The string for checking
 * @return true The string is number
 */
bool isStrNum(const char *str)
{
    if(str == NULL)
	{
	    writeToLog("ERROR: isStrNum(): the given string is NULL", TAG);
	    return false;
	}
    size_t len = strlen(str);
    if(len == 0)
	{
	    writeToLog("ERROR: isStrNum(): the given string is empty", TAG);
	    return false;	    
	}
    int i;
    for(i = 0; i < len; ++i)
	{
	    if(!isdigit(str[i]))
		{
		    writeToLog2("ERROR: isStrNum(): the given string contains a non-digit character. The string: ", str, TAG);
		    return false;
		}
	}
    return true;
}

/**
 * Fill the structures of a host info
 * @param host_info The host info structure, which should be filled up
 * @param host_info_list The list of the host info structs
 * @return The result: ERR or NO_ERR
 */
const int fillHostInfoStructs(struct addrinfo *host_info, struct addrinfo **host_info_list)
{
    writeToLog("Setting up the structs...\n", TAG);

    host_info->ai_family   = AF_UNSPEC;     // IP version not specified. Can be both.
    host_info->ai_socktype = SOCK_STREAM;   // Use SOCK_STREAM for TCP or SOCK_DGRAM for UDP.
    host_info->ai_flags    = AI_PASSIVE;    // IP Wildcard

    if(!isStrNum(portNum))
	return ERR;
	
    const int status = getaddrinfo(NULL, portNum, host_info, host_info_list);

    if (status != NO_ERR)
	writeToLog2("\tERROR fillHostInfoStructs(): ", gai_strerror(status), TAG);

    return status;
}

/**
 * Create socket from the given list of the host info structs
 * @param host_info_list The list of the host info structs
 * @return The socket descriptor
 */
const int createSocket(struct addrinfo *host_info_list)
{
    writeToLog("Creating a socket...\n", TAG);
    
    const int socketfd = socket(host_info_list->ai_family, host_info_list->ai_socktype,
                                host_info_list->ai_protocol);
    writeToLogIfError(socketfd, "createSocket(): ", strerror(errno), TAG);
    
    return socketfd;
}

/**
 * Bind the socket with the given descriptor
 * @param host_info_list The list of the host info structs
 * @param sockDescr The socket descriptor
 * @return The status of binding
 */
const int bindSocket(const int sockDescr, struct addrinfo *host_info_list)
{
    writeToLog("Binding socket...\n", TAG);

    int yes = 1;
    int status = setsockopt(sockDescr, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    if (status == ERR)
	{
	    writeToLog2("\tERROR bindSocket(): ", gai_strerror(status), TAG);
	    return status;
	}

    //    struct timeval timeout;      
    //    timeout.tv_sec = SOCKET_TIME_OUT;
    //    timeout.tv_usec = 0;

    //    status = setsockopt(sockDescr, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
    //    if (status < 0)
    //	{
    //	    writeToLog2("\tERROR bindSocket(): ", gai_strerror(status), TAG);
    //	    return status;
    //	}

    status = bind(sockDescr, host_info_list->ai_addr, host_info_list->ai_addrlen);
    writeToLogIfError(status, __func__, gai_strerror(status), TAG);

    return status;
}

/**
 * Listen to new connections
 * @param sockDescr A socket's descriptor
 * @return ERR if there is an error. Any other number if there is no error
 */
const int listenConns(const int sockDescr)
{
    writeToLog("Listening for connections...\n", TAG);

    const int status =  listen(sockDescr, MAX_SOCKETS_NUM_WAITED_FOR_ACCEPT);
    writeToLogIfError(status, "listenConns(): ", strerror(errno), TAG);

   // add the listener to the master set
    FD_SET(sockDescr, &master);

    // keep track of the biggest file descriptor
    fdmax = sockDescr; // so far, it's this one

    return status;
}

/**
 * Accept a connection
 * @param sockDescr A socket's descriptor
 * @return ERR if there is an error. Any other number if there is no error
 */
const int acceptConn(const int sockDescr)
{
    struct sockaddr_storage their_addr;
    
    socklen_t addr_size = sizeof(their_addr);
    const int newSockDescr = accept(sockDescr, (struct sockaddr *)&their_addr, &addr_size);

    writeToLogIfError(newSockDescr, "acceptConn(): ", strerror(errno), TAG);
    
    return newSockDescr;
}

/**
 * Receive a data from a client
 * @param newSockDescr A socket's descriptor
 * @param incoming_data_buffer The buffer for incoming data
 * @param buff_len The length of the buffer
 * @return The buffer of the incoming data or NULL if there is an error
 */
const char* receiveData(const int newSockDescr, char* incoming_data_buffer, const size_t buff_len)
{
    if(incoming_data_buffer == NULL)
      {
	  writeToLog("ERROR receiveData(): The given pointer to buffer for incoming data is NULL\n", TAG);
	  return NULL;
      }
    if(buff_len == 0)
      {
	  writeToLog("ERROR receiveData(): The length of the given buffer is 0\n", TAG);
	return NULL;
      }

    if(buff_len != 0)
	bzero(incoming_data_buffer, buff_len);
    
    writeToLog("Waiting to receive data...\n", TAG);

    ssize_t bytes_recieved = recv(newSockDescr, incoming_data_buffer, buff_len, 0);
    
    if (bytes_recieved == 0)
	{
	    writeToLog("\tHost shut down.\n", TAG);
	    return STR_END;
	}
    if (bytes_recieved == ERR) 
      {
	  writeToLog2("\tERROR receiveData(): ", strerror(errno), TAG);
	  return NULL;
      }
    
    incoming_data_buffer[bytes_recieved - 2] = '\0';    // remove EOL

    return incoming_data_buffer;
}

/**
 * Send data by sockets.
 * @param socketDescr The socket descriptor
 * @return ERR or NO_ERR
 */
const int sendData(const int socketDescr)
{
    char* sentData = getSentData();
    writeToLog2("Sending the answer: ", sentData, TAG);

    const size_t sentDataLen = strlen(sentData);
    const ssize_t bytes_sent = send(socketDescr, sentData, sentDataLen, 0);

    bzero(sentData, sentDataLen);

     if(bytes_sent == ERR)
	{
	    writeToLog2("\tERROR sendData(): ", strerror(errno), TAG);
	    return ERR;
	}

    if(bytes_sent != sentDataLen)
	{
	    writeToLog("\tERROR sendData(): The data hasn't sent successfully. The size of the sent data doesn't equal the actual data's size\n", TAG);
	    return ERR;
	}

    return NO_ERR;
}   

/**
 * Close the socket connection
 * @param sockDescr The socket descriptor of the connection
 * @return The result integer of closing
 */
const int closeSocketConn(const int sockDescr)
{
    if(sockDescr <= 0)
	return ERR;
	
    writeToLog("\tClosing socket connection\n", TAG);

    const int res = close(sockDescr);
    if(res == ERR)
	writeToLog2("\tERROR closeSocketConn(): ", strerror(errno), TAG);

    FD_CLR(sockDescr, &master);
    
    return res;
}

/**
 * Initial connection before listening
 * @return socket descriptor or ERR
 */
const int initConnectionBeforeListen()
{
    struct addrinfo host_info;       // The struct that getaddrinfo() fills up with data.
    struct addrinfo *host_info_list; // Pointer to the to the linked list of host_info's.
    memset(&host_info, 0, sizeof host_info);

    FD_ZERO(&master);    // clear the master and temp sets
    FD_ZERO(&read_fds);
    
    int sockDescr = ERR;
    if(fillHostInfoStructs(&host_info, &host_info_list) != ERR)
	{
	    sockDescr = createSocket(host_info_list);
	    if(sockDescr != ERR)
		{
		    if (bindSocket(sockDescr, host_info_list) != ERR)
		    {
			    bzero(localIP, IP_ADDR_STR_LEN);
			    copyLocalIp2Str(localIP);
		    }
		    else
		    {
		    	closeSocketConn(sockDescr);
		    	sockDescr = ERR;
		    }
		}
	}

    freeaddrinfo(host_info_list);
    return sockDescr;
}

/**
 * Client-server conversation
 * @param incoming_data_buffer The string buffer for the incoming from the client data
 * @param buff_len The length of the buffer
 * @param listenSocketDescr The descriptor of the listening socket
 * @param curSocketDescr Currently used socket descriptor 
 * @return The integer status of the conversation: ERR, NO_ERR or STOP if received the connection
 *                                                                     end message
 */
const int conversation(char *incoming_data_buffer, const size_t buff_len, const int listenSocketDescr, const int curSocketDescr)
{
    if(incoming_data_buffer == NULL)
      {
	  FD_CLR(curSocketDescr, &master);
	  writeToLog("ERROR conversation(): The given pointer to buffer for incoming data is NULL\n", TAG);
	  return ERR;
      }
    if(buff_len == 0)
      {
	  FD_CLR(curSocketDescr, &master);
	  writeToLog("ERROR conversation(): The length of the given buffer is 0\n", TAG);
	  return ERR;
      }

    const char* str = receiveData(curSocketDescr, incoming_data_buffer, buff_len);
    if(str == NULL)
	{
	    FD_CLR(curSocketDescr, &master);
	    writeToLog2(__func__, "ERROR: The received string is NULL\n", TAG);
	    return ERR;
	}
    if(strcmp(str, STR_END) == 0)
	{
	    FD_CLR(curSocketDescr, &master);
	    writeToLog2(__func__, ": The received string is EMPTY\n", TAG);
	    return STOP;
	}

    writeToLog2("\tReceived string: ", incoming_data_buffer, TAG);
 
    setReceivedData(incoming_data_buffer);
    setReceivedDataStatus(HAS_NEW_DATA);

    while(strlen(getSentData()) == 0);   // waiting for the input of the data for sending

    int result = ERR;
    int j;
    for(j = 0; j <= fdmax; ++j)
	{
	    if(FD_ISSET(j, &master))
		{
		    if( (j != listenSocketDescr) )
			result = sendData(j);
		}
	}
    return result;
}



/**
 * Run client-server connection
 * @param sockDescr An initialized socket descriptor
 */
void runConnection(const int sockDescr)
{
    if(sockDescr == ERR)
	return;

    if (listenConns(sockDescr) == ERR)
	{
	    closeSocketConn(sockDescr);
	    return;
	}
    
    initMutexes();
    
    int newSockDescr = ERR;
    while( (getRunStatus() != STOP))
    {
	read_fds = master;
	if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == ERR)
	    break;

	int i;
	for(i = 0; i <= fdmax; i++)
	    {
		if (FD_ISSET(i, &read_fds))
		    {
			if(i == sockDescr)
			    {
				newSockDescr = acceptConn(sockDescr);
				if(newSockDescr != ERR)
				    {
					FD_SET(newSockDescr, &master); // add to master set
					if (newSockDescr > fdmax)
					    fdmax = newSockDescr;
					
					bzero(connectedIP, IP_ADDR_STR_LEN);
					copyConnectedIp2Str(newSockDescr, connectedIP);		
				    }
			    }
			else
			    {
				char receivedDataArr[DATA_LEN] = {'\0'};
				int result = conversation(receivedDataArr, DATA_LEN, sockDescr, i);
				if( (result == ERR) || (result == STOP) )
				    {
					closeSocketConn(newSockDescr);
					break;
				    }
			    }
		    }
	    }
	usleep(MILLISECONDS_SLEEP_TIME * 1000); 
    }

    closeSocketConn(sockDescr);

    destroyMutexes();
}
