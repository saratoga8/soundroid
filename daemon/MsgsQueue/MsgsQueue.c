/**
 * @file
 * Linux Messages Queue for interprocess communication
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

#include <errno.h>
#include <string.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>


#define TAG "MSGS_QUEUE" /**< The tag for writing to log file */


/**
 * Create the file of messages queue
 * @return true The file has created successfully
 */
bool createQueueMsgsFile()
{
  FILE *file = fopen(FILE_NAME, "w+");
  if(file == NULL)
    {
	writeToLog2("ERROR: createQueueMsgsFile(): Can't create the file for the messages queue: ", strerror(errno), TAG);
	return false;
    }

  if(fclose(file) == EOF)
    {
	writeToLog2("ERROR: createQueueMsgsFile(): Can't close the file for the messages queue: ", strerror(errno), TAG);
	return false;
    }
  return true;
}

/**
 * Does the file of messages queue exist?
 * @return true The file does exist
 */
bool doesQueueMsgsFileExist()
{
    return (access(FILE_NAME, R_OK | W_OK) != ERROR);
}

/**
 * Initialize the the queue of messages
 * @param permissionFlag The flag of the queue permission
 * @return The messages queue ID
 */
const int initMsgsQueue(const int permissionFlag)
{    
  const key_t key = ftok(FILE_NAME, 'B');
  
  if (key == ERROR)
    {
	writeToLog2("ERROR initMsgsQueue(): Can't create messages queue: ", strerror(errno), TAG);
      return false;
    }

  int msgid = ERROR;
  if ( (msgid = msgget(key, permissionFlag)) == ERROR )
      writeToLog2("ERROR initMsgsQueue(): Can't create messages queue: ", strerror(errno), TAG);

  return msgid;
}
