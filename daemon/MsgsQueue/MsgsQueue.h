/**
 * @file 
 * Messages queue for IPC in Linux
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

#ifndef MSGSQUEUE_H_
#define MSGSQUEUE_H_

#include <stdbool.h>

#define MSG_STR_MAX_LEN 50         /**< The maximal length of a message */
#define FILE_NAME "queue.msgs"     /**< The file name for messages queue */
#define ERROR -1                   /**< The code of an error */

/**
 * \struct message
 * \brief The structure of a message
 */
struct message
{
    long mtype;
    char mtxt[MSG_STR_MAX_LEN];
};

/**
 * \struct MsgsQueueData
 * \brief the data of a queue message
 */
struct MsgsQueueData
{
    int queueID;                                    /**< The ID of the messages queue, that should be initialized */
    char receivedMsgTxt[MSG_STR_MAX_LEN];           /**< The string array of a message's text */    
};

/**
 * Does the file of messages queue exist?
 * @return true The file does exist
 */
bool doesQueueMsgsFileExist();

/**
 * Create the file of messages queue
 * @return true The file has created successfully
 */
bool createQueueMsgsFile();

/**
 * Initialize the the queue of messages
 * @param permissionFlag The flag of the queue permission
 * @return The messages queue ID
 */
const int initMsgsQueue(const int permissionFlag);
    
#endif
