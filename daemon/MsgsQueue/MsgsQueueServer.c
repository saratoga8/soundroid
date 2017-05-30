/**
 * @file
 *  Messages queue for IPC in Linux, the server side
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

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <errno.h>

#include <stdbool.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>

#include <Log.h>

#include "MsgsQueue.h"

#include <pthread.h>

#define TAG "MSGS_QUEUE_SERVER" /**< the tag for writing to log file */

#define HAS_NEW_MSG 4                       /**< there is a new message in queue */
#define NO_NEW_MSG  5                       /**< there are no new  messages in queue */

int receivingMsgStatus = NO_NEW_MSG;        /**< the server's receiving status */



/**
 * /struct msg
 *  The structure of a message
 */
struct message msg;    

#define RECEIVING 1                      /**< receiving a message */
#define SENDING   2                      /**< sending a message */
#define STOP      3                      /**< stopping the queue */

int runingStatus = RECEIVING;            /**< the running status of the messages server */

pthread_mutex_t statusMutex;             /**< the mutex of the server's status */

struct MsgsQueueData serverQueueData;


/**
 * Set the running status
 * @param status The status code: RECEIVING, SENDING or STOP
 */
void setRunningStatus(const int status)
{
    pthread_mutex_lock   (&statusMutex);
    runingStatus = status;
    pthread_mutex_unlock (&statusMutex);
}

/**
 * Send message from the server's side
 * @param msgTxt The text of the message
 * @return true The message has sent successfully
 */
bool sendMsgServer(const char *msgTxt)
{
    if(msgTxt == NULL)
	{
	    writeToLog("ERROR sndMsgServer(): Can't send message, the given message text is NULL\n", TAG);
	    return false;
	}
    if(strlen(msgTxt) == 0)
	{
	    writeToLog("ERROR sndMsgServer(): Can't send message, the given message text is empty\n", TAG);
	    return false;
	}
    
    setRunningStatus(SENDING);

    struct message answer;
    bzero(answer.mtxt, MSG_STR_MAX_LEN);
    answer.mtype = 1;
    strcpy(answer.mtxt, msgTxt);
    
    if (msgsnd(serverQueueData.queueID, &answer, sizeof(answer.mtxt), 0) == ERROR) /* +1 for '\0' */
	{
	    writeToLog2("ERROR sndMsgServer(): Can't send a messages to queue: ", strerror(errno), TAG);
	    setRunningStatus(STOP);  
	    return false;
	}
    
    setRunningStatus(RECEIVING);  
    return true;
}

/**
 * Receive a message on the server's side
 * @return The text of the received message
 */
char* receiveMsgServer()
{    
    pthread_mutex_lock   (&statusMutex);
    if(receivingMsgStatus == HAS_NEW_MSG)	
	{
	    receivingMsgStatus = NO_NEW_MSG;
	    pthread_mutex_unlock (&statusMutex);
	    return serverQueueData.receivedMsgTxt;
	}
    pthread_mutex_unlock (&statusMutex);
    return "";
}

/**
 * Initialize the messages queue on the server's side
 * @return true Initialized successfully
 */
bool initQueueServer()
{
    bzero(serverQueueData.receivedMsgTxt, MSG_STR_MAX_LEN);
    bzero(msg.mtxt, MSG_STR_MAX_LEN);
    
    if(createQueueMsgsFile())
	serverQueueData.queueID = initMsgsQueue(0644 | IPC_CREAT);

    return (serverQueueData.queueID != ERROR);
}

/**
 * Get the running status of the server's side messages queue
 * @return The status code: RECEIVING, SENDING or STOP
 */
const int getRunningStatus()
{
    int status;
    
    pthread_mutex_lock   (&statusMutex);
    status = runingStatus;
    pthread_mutex_unlock (&statusMutex);
    
    return status;
}

/**
 * Run the messages queue
 */
void runQueue()
{
    while(getRunningStatus() != STOP)
	{
	    if (getRunningStatus() == SENDING)
		continue;
	    if (getRunningStatus() == RECEIVING)
		{
		    if (msgrcv(serverQueueData.queueID, &msg, sizeof(msg.mtxt), 0, 0) != ERROR)
			{
			    strcpy(serverQueueData.receivedMsgTxt, msg.mtxt);

			    pthread_mutex_lock   (&statusMutex);
			    receivingMsgStatus = HAS_NEW_MSG;
			    pthread_mutex_unlock (&statusMutex);
			    
			    bzero(msg.mtxt, MSG_STR_MAX_LEN);

			}
		    else if(errno != EIDRM)
			{
			    writeToLog2("ERROR runQueue(): Can't get a messages from queue: ", strerror(errno), TAG);
			    break;
			}
		}
	}
}

/**
 * Stop the messages queue and delete it
 */
void deleteMsgsQueue()
{
    setRunningStatus(STOP);
    sleep(1);

    if (msgctl(serverQueueData.queueID, IPC_RMID, NULL) == ERROR) {
	writeToLog2("ERROR deleteMsgsQueue(): Can't delete the messages queue: ", strerror(errno), TAG);
  }
}

/**
 * Delete the file of the messages queue
 */
void deleteQueueMsgsFile()
{
  if(remove(FILE_NAME) == ERROR)
    {
	writeToLog2("ERROR: createQueueMsgsFile(): Can't remove the file for the messages queue: ", strerror(errno), TAG);
    }
}

/**
 * Delete messages queue and its file
 */
void deleteQueue()
{
    deleteMsgsQueue();
    deleteQueueMsgsFile();
}
