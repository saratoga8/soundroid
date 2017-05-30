/**
 * @file
 * The closing client should send the closing message to the server,
 * to stop its running
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
 
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#include "Log.h"

#define NO_ERR 0    /**< No errors */
#define ERR   -1    /**< There is an error */
 
#define TAG "CLOSING_CLIENT"   /**< The tag for writing to log file */

#define MAXDATASIZE 100 /**< The max number of bytes we can get at once  */


/**
 * Fill the given structure of server info
 * @param port The port number string for listening
 * @param servinfo The poiner to the structure of server info
 * @return The status: ERR or NO_ERR
 */
const int fillServInfoStructs(const char *port, struct addrinfo **servinfo)
{
    struct addrinfo hints;    
    memset(&hints, 0, sizeof hints);
    
    hints.ai_family   = AF_UNSPEC;     // IP version not specified. Can be both.
    hints.ai_socktype = SOCK_STREAM;   // Use SOCK_STREAM for TCP or SOCK_DGRAM for UDP.

    const int status = getaddrinfo("localhost", port, &hints, servinfo);

    if (status != NO_ERR)
	writeToLog2("\tERROR fillHostInfoStructs(): ", gai_strerror(status), TAG);

    return status;
}

/**
 * Initialize connection
 * @param servinfo The poiner to the structure of server info
 * @return The status: ERR or NO_ERR
 */ 
const int initConnection(struct addrinfo *servinfo)
{
    int sockfd = ERR;
    struct addrinfo *p = NULL;
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == ERR) {
	    writeToLog2("\tERROR init socket: ", strerror(errno), TAG);
            continue;
        }
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == ERR) {
	    writeToLog2("\tERROR try to connect: ", strerror(errno), TAG);
	    if(close(sockfd) == ERR)
		writeToLog2("\tERROR can't close socket: ", strerror(errno), TAG);
            continue;
        }
        break;
    }

    return (p == NULL) ? ERR: sockfd;
}

/**
 * Send the end message to the given socket descriptor
 * @param sockfd The file descriptor of the socket for sending message
 */
void sendEnd(const int sockfd)
{
#define MSG "end\n\r"
    const size_t msg_len = strlen(MSG);
    
    const ssize_t sent_bytes = send(sockfd, MSG, msg_len, 0);
    
    if(sent_bytes == ERR)
	writeToLog2("\tERROR can't send message: ", strerror(errno), TAG);
    if(sent_bytes != msg_len)
	writeToLog2("\tERROR can't send message: ", strerror(errno), TAG);    
}
    
/**
 * Run the client for closing
 * @param port The port number string of client
 * @return The status: ERR or NO_ERR
 */
const int runClosingClient(const char *port)
{
    struct addrinfo *servinfo;
    
    if (fillServInfoStructs(port, &servinfo) == ERR) {
	freeaddrinfo(servinfo);
	return ERR;
    }
    
    int sockfd = initConnection(servinfo);
    
    freeaddrinfo(servinfo);
    
    if(sockfd == ERR)
	return ERR;

    sendEnd(sockfd);
    
    if(close(sockfd) == ERR)
	{
	    writeToLog2("\tERROR can't close socket: ", strerror(errno), TAG);
	    return ERR;
	}

    return NO_ERR;
}
