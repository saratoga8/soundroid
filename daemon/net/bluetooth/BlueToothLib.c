/**
 * @file
 * The library for client-server connection by blue tooth
 * Sending text messages and getting addresses of the local blue tooth adapter
 * and a connected one
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


#include "service.h"

#include "BlueToothLib.h"
#include "synchronise.h"
#include "Log.h"
#include "addr.h"

#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include <sys/socket.h>
#include <netdb.h>



#define STR_END "end"
#define TAG "BT_LIB"                                         /**< The tag for writing to the log file */

#define MAX_SOCKETS_NUM_WAITED_FOR_ACCEPT 1                  /**< The maximal number of sockets waiting for accept */

#define MAX_BT_DEV_NAME_LEN 50                               /**< The maximal length of bluetooth device */
char connectedAdapterName [MAX_BT_DEV_NAME_LEN] = { 0 };     /**< The string of the name of a connected device */
char localAdapterName     [MAX_BT_DEV_NAME_LEN] = { 0 };     /**< The string of the name of the local adapter */

/**
 * Get local address string
 * @return The string of the local address or ""
 */
const char* getLocalAddr()
{
    writeToLog2("Local addr: ", localAdapterName, TAG);
    return localAdapterName;
}

/**
 * Get connected address string
 * @return The string of the connected address or ""
 */
const char* getConnectedAddr()
{
    return connectedAdapterName;
}


/**
 * Close the socket connection
 * @param sockDescr The socket descriptor of the connection
 * @return The result integer of closing
 */
const int closeBtSocketConn(const int sockDescr)
{
    if(sockDescr <= 0)
	return ERR;
	
    writeToLog("\tClosing socket connection\n", TAG);

    const int res = close(sockDescr);
    writeToLogIfError(res, __func__, strerror(errno), TAG);

    return res;
}

/**
 * Create blue tooth socket
 * @return The socket descriptor
 */
const int createBtSocket()
{    
    writeToLog("Creating a socket...\n", TAG);
    
    const int socketfd = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    writeToLogIfError(socketfd, __func__, strerror(errno), TAG);
    if(socketfd != ERR)
	{
	    if(fcntl(socketfd, F_SETFL, O_NONBLOCK) == ERR)
		{
		    closeBtSocketConn(socketfd);
		    writeToLog2("Can't set the socket non-blocking: ", strerror(errno), TAG);
		    return ERR;
		}
	}
    
    return socketfd;
}

/**
 * Bind the socket with the given descriptor
 * @param sockDescr The socket descriptor
 * @return The status of binding
 */
const int bindBtSocket(const int sockDescr)
{    
    writeToLog("Binding socket...\n", TAG);
    
    struct sockaddr_rc loc_addr = { 0 };
    
    loc_addr.rc_family = AF_BLUETOOTH;
    loc_addr.rc_bdaddr = *BDADDR_ANY;
    loc_addr.rc_channel = (uint8_t) 11;

    const int status = bind(sockDescr, (struct sockaddr*) &loc_addr, sizeof(loc_addr));
    writeToLogIfError(status, __func__, gai_strerror(status), TAG);

    return status;
}

/**
 * Initialize the name of the local bluetooth adapter
 */
void initLocalAdapterName()
{
    const int adapterID      = hci_get_route(NULL);
    const int adapterHandler = hci_open_dev(adapterID);
    if(adapterHandler < 0)
	{
	    writeToLog2("ERROR: Can't open the local adapter:", strerror(errno), TAG);
	    return;	    
	}
    if (hci_read_local_name(adapterHandler, MAX_BT_DEV_NAME_LEN, localAdapterName, 2000) < 0) {
	writeToLog2("ERROR: Can't get the local adapter's name: ", strerror(errno), TAG);
	return;
    }
    close(adapterHandler);
}

/**
 * Initialize the connection before listening
 * @return socket descriptor or ERR
 */
const int initBtConnectionBeforeListen()
{
    const int socket = createBtSocket();
    if(socket != ERR)
	{
	    if(bindBtSocket(socket) == ERR)
		{
		    closeBtSocketConn(socket);
		    return ERR;		    
		}
	}
    initLocalAdapterName();
    return socket;
}

/**
 * Listen to new connections
 * @param sockDescr A socket's descriptor
 * @return ERR if there is an error. Any other number if there is no error
 */
const int listenBtConns(const int sockDescr)
{
    writeToLog("Listening for connections...\n", TAG);

    const int status = listen(sockDescr, MAX_SOCKETS_NUM_WAITED_FOR_ACCEPT);
    writeToLogIfError(status, __func__, strerror(errno), TAG);    
    
    return status;
}

/**
 * Accept a connection
 * @param sockDescr A socket's descriptor
 * @return ERR if there is an error. Any other number if there is no error
 */
const int acceptBtConn(const int sockDescr)
{
    writeToLog("Accepting connections...\n", TAG);
    
    struct sockaddr_rc rem_addr = { 0 };
    socklen_t opt = sizeof(rem_addr);
    int newSockDescr = accept(sockDescr, (struct sockaddr *)&rem_addr, &opt);
    while(newSockDescr == ERR)	
	{	    
	    if(errno == EWOULDBLOCK)
		{
		    if(getRunStatus() == STOP)
			return STOP;
		}
	    else
		break;
	    usleep(MILLISECONDS_SLEEP_TIME * 1000);
	    newSockDescr = accept(sockDescr, (struct sockaddr *)&rem_addr, &opt);
	}
									     
    writeToLogIfError(newSockDescr, __func__, strerror(errno), TAG);

    const int adapterID      = hci_get_route(NULL);
    const int adapterHandler = hci_open_dev(adapterID);
    if(adapterHandler >= 0)
	{	
	    if (hci_read_remote_name(adapterHandler, &rem_addr.rc_bdaddr, MAX_BT_DEV_NAME_LEN, connectedAdapterName, 0) < 0)
		writeToLog2("ERROR: Can't get the remote adapter's name: ", strerror(errno), TAG);
	}
    else
	writeToLog2("ERROR: Can't open the local adapter:", strerror(errno), TAG);
    
    close(adapterHandler);

    writeToLog2("accepted connection from ", connectedAdapterName, TAG);
    
    return newSockDescr;
}

/**
 * Receive a data from a client
 * @param newSockDescr A socket's descriptor
 * @param incoming_data_buffer The buffer for incoming data
 * @param buff_len The length of the data's buffer
 * @return The buffer of the incoming data or NULL if there is an error
 */
const char* receiveBtData(const int newSockDescr, char* incoming_data_buffer, const size_t buff_len)
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
    while(bytes_recieved == ERR)	
	{	    
	    if(errno == EWOULDBLOCK)
		{
		    if(getRunStatus() == STOP)
			return STR_END;
		}
	    else
		break;
	    usleep(MILLISECONDS_SLEEP_TIME * 1000);
	    bytes_recieved = recv(newSockDescr, incoming_data_buffer, buff_len, 0);
	}
    
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
    
    incoming_data_buffer[bytes_recieved] = '\0';    // remove EOL

    return incoming_data_buffer;
}

/**
 * Send data by sockets.
 * @param socketDescr The socket descriptor
 * @return ERR or NO_ERR
 */
const int sendBtData(const int socketDescr)
{
    char* sentData = getSentData();
    writeToLog2("Sending the answer: ", sentData, TAG);

    const size_t sentDataLen = strlen(sentData);
    const ssize_t bytes_sent = write(socketDescr, sentData, sentDataLen);

    bzero(sentData, sentDataLen);
    
    if(bytes_sent == ERR)
	{
	    writeToLog2("\tERROR sendBtData(): ", strerror(errno), TAG);
	    return ERR;
	}

    if(bytes_sent != sentDataLen)
	{
	    writeToLog("\tERROR sendBtData(): The data hasn't sent successfully. The size of the sent data doesn't equal the actual data's size\n", TAG);
	    return ERR;
	}

    return NO_ERR;
}

/**
 * Client-server conversation
 * @param incoming_data_buffer The string buffer for the incoming from the client data
 * @param buff_len The length of the data(send/receive) buffer
 * @param socket The socket descriptor of the conversation
 * @return The integer status of the conversation: ERR, NO_ERR or STOP if received the connection
 *                                                                     end message
 */
const int conversationBt(char *incoming_data_buffer, const size_t buff_len, const int socket)
{
    if(incoming_data_buffer == NULL)
      {
	  writeToLog("ERROR conversationBt(): The given pointer to buffer for incoming data is NULL\n", TAG);
	  return ERR;
      }
    if(buff_len == 0)
      {
	  writeToLog("ERROR conversationBt(): The length of the given buffer is 0\n", TAG);
	  return ERR;
      }

    const char* str = receiveBtData(socket, incoming_data_buffer, buff_len);
    if(str == NULL) 
    {
	writeToLog("ERROR conversationBt(): the received data is NULL\n", TAG);
	return ERR;
    }
    if(strcmp(str, STR_END) == 0)
    {
	writeToLog("ERROR conversationBt(): the received data is EMTPTY\n", TAG);
        return STOP;
    }

    writeToLog2("\tReceived string: ", incoming_data_buffer, TAG);
 
    setReceivedData(incoming_data_buffer);
    setReceivedDataStatus(HAS_NEW_DATA);
    
    while(strlen(getSentData()) == 0);   // waiting for the input of the data for sending

    return sendBtData(socket);
}

/**
 * Run client-server connection
 * @param sockDescr Socket deskriptor of the server
 */
void runBtConnection(const int sockDescr)
{
    if(sockDescr == ERR)
	return;
    
    if(listenBtConns(sockDescr) == ERR)
	{
	    closeBtSocketConn(sockDescr);
	    return;
	}
        
    initMutexes();

    char receivedDataArr[DATA_LEN] = {'\0'};
    int newSockDescr;
    while( (getRunStatus() != STOP) )
	{
	    bzero(connectedAdapterName, sizeof(char));
	    newSockDescr = acceptBtConn(sockDescr);
	    if( (newSockDescr != ERR))
		{
		    bzero(receivedDataArr, sizeof(char));
		    int result = NO_ERR;
		    while(result == NO_ERR)
			{
			    result = conversationBt(receivedDataArr, DATA_LEN, newSockDescr);
			    if( (result == ERR) || (result == STOP) )
				{
				    closeBtSocketConn(newSockDescr);
				    break;
				}
			}
		}
	}
    
    closeBtSocketConn(sockDescr);
    
    destroyMutexes();    
}
