/**
 * @file
 * Port operations from GUI.
 * All the port operations from GUI realised by
 * by sending messages with commands to the system sound control daemon
 * (sending messages is based on the inter process communication messages queue)
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

#include "CommandsNames.h"
#include "MsgsQueueClient.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/**
 * Send the given string in messages to the server side of the messages queue
 * which is run in the daemon
 * @param str The string for sending
 * @return The response string from the server side of messages queue.
 *         If there is an error, the "" will be returned
 */
char *sendStrInQueueMsg(const char* str)
{
    if(str == NULL)
	fprintf(stderr, "ERROR: the given string for sending to messages queue is NULL\n");
    else if(strlen(str) == 0)
	fprintf(stderr, "ERROR: the given string for sending to messages queue is empty\n");
    else if(initQueueClient())
	{
	    if(sendMsgClient(str))
		{
		    char *response = receiveMsgClient();
		    return (strcmp(response, "ERR") == 0) ? "": response;
		}
	    else
		fprintf(stderr, "ERROR: Can't send msg to the messages queue\n");
	}
    return "";
}

/**
 * Get currently used port of daemon
 * @return The string of the used port number
 */
char* getPortOfDaemon()
{
    char *response = sendStrInQueueMsg(GET_PORT);
    return (strcmp(response, ERR) != 0) ? response: "";
}

/**
 * Build the string of a command with parameters for sending through the messages queue
 * @param commandName The name of the command
 * @param param The string of parameters
 * @return The built string
 */
char* buildCommandStr(const char *commandName, const char* param)
{
    size_t len = strlen(commandName) + strlen(param) + 2;
    char *command = (char*)calloc(sizeof(char), len);
    strcpy(command, commandName);
    strcat(command, " ");
    return strcat(command, param);
}

/**
 * Change the port of the daemon to the given port number
 * @param newPortNum The new port number
 * @return true - The change of the port has been successful
 */
bool chgPortOfDaemon(const char *newPortNum)
{
    bool result = false;
    if(initQueueClient()) 
	{
	    char *command = buildCommandStr(CHG_PORT, newPortNum);
	    if(sendMsgClient(command))
		{
		    char *response = receiveMsgClient();
		    if(strcmp(response, "OK") == 0)
			result = true;
		    else    
			fprintf(stderr, "ERROR: The received message is error\n");
		}
	    else
		fprintf(stderr, "ERROR: Can't send msg to the messages queue\n");
	    free(command);
	}
    return result;
}

/**
 * Get local IP by sending message with command to the server side
 * of the messages queue opened by the running
 * volume control daemon and getting a message
 * with IP from one.
 * @return The local IP string 
 */
char* getLocalIP()
{
    return sendStrInQueueMsg(LOCAL_IP);
}

/**
 * Get the IP of a connected client
 * by sending message with command to the server side
 * of the messages queue opened by the running
 * volume control daemon and getting a message
 * with IP from one.
 * @return The connected IP string 
 */
char* getConnectedIP()
{
    return sendStrInQueueMsg(CONNECTED_IP);
}


