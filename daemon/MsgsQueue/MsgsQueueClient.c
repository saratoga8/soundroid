/**
 * @file
 * The messages queue of the client side
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

#include "MsgsQueue.h"
#include "Log.h"

#include "string.h"
#include "errno.h"
#include "strings.h"

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define TAG "MSGS_QUEUE_CLIENT"                /**< The tag for writing to log file */


struct MsgsQueueData clientQueueData;

/**
 * Initialize the messages queue of the client side
 * @return true The queue has initialized successfully
 */
bool initQueueClient()
{    
    bzero(clientQueueData.receivedMsgTxt, MSG_STR_MAX_LEN);

    if(doesQueueMsgsFileExist())
	clientQueueData.queueID = initMsgsQueue(0644);
    else
	return false;

    return clientQueueData.queueID != ERROR;
}

/**
 * Send message with the given text from the clien side to the server one
 * @param msgTxt The text of the message
 * @return true The message has sent successfully
 */
bool sendMsgClient(const char *msgTxt)
{
    if(msgTxt == NULL)
	{
	    writeToLog("ERROR sendMsgClient(): Can't send message, the given message text is NULL", TAG);
	    return false;
	}
    if(strlen(msgTxt) == 0)
	{
	    writeToLog("ERROR sndMsgClient(): Can't send message, the given message text is empty", TAG);
	    return false;
	}
    
    struct message answer;
    bzero(answer.mtxt, MSG_STR_MAX_LEN);
    answer.mtype = 1;
    strcpy(answer.mtxt, msgTxt);
    
    if (msgsnd(clientQueueData.queueID, &answer, sizeof(answer.mtxt), 0) == ERROR) /* +1 for '\0' */
	{
	    writeToLog2("ERROR sndMsgClient(): Can't send a messages to queue: ", strerror(errno), TAG);
	    return false;
	}
    return true;
}

/**
 * Receive the message on the client side from the server one
 * @return The text of the message
 */
char* receiveMsgClient()
{
    struct message msg;
    bzero(clientQueueData.receivedMsgTxt, MSG_STR_MAX_LEN);
    
    if (msgrcv(clientQueueData.queueID, &msg, sizeof(msg.mtxt), 0, 0) == ERROR)
	writeToLog2("ERROR receiveMsgClient(): Can't receive a messages from queue: ", strerror(errno), TAG);
    else
	strcpy(clientQueueData.receivedMsgTxt, msg.mtxt);

    return clientQueueData.receivedMsgTxt;
}


