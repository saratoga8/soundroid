/**
 * @file
 * Threads synchronisation of sending and receiving data by used connection type
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


#include "synchronise.h"

#include <string.h>

int runStatus = RUN;                       /**< The run status: running or stopped */

int  receivedDataStatus = NO_NEW_DATA;     /**< The status of receiving data */

char sentData[DATA_LEN] = {'\0'};          /**< The buffer for the sent data */

char receivedData[DATA_LEN] = {'\0'};      /**< The buffer for the received data */

pthread_mutex_t mutexData;                 /**< The mutex for synchronising data */
pthread_mutex_t mutexStop;                 /**< The mutex for synchronising running */

#define TAG "synchronise"                  /**< The tag for logs */

/**
 * Destroy mutexes
 */
void destroyMutexes()
{
    pthread_mutex_destroy(&mutexStop);
    pthread_mutex_destroy(&mutexData);
}

/**
 * Initialize mutexes
 */
void initMutexes()
{
    pthread_mutex_init(&mutexStop, NULL);
    pthread_mutex_init(&mutexData, NULL);    
}

/**
 * Get pointer to the sent data string
 * @return The string of the sent data
 */
char* getSentData()
{
    return sentData;
}

/**
 * Set data string for sending
 * @param dataStr data string
 */
void setSentData(const char *dataStr)
{
    pthread_mutex_lock(&mutexData);
    if(strlen(sentData) != 0)
	bzero(sentData, DATA_LEN);
    strcpy(sentData, dataStr);
    pthread_mutex_unlock(&mutexData);
}

/**
 * Set received data string by the given string
 * @param dataStr The string for setting received data
 */
void setReceivedData(const char *dataStr)
{
    pthread_mutex_lock(&mutexData);
    strcpy(receivedData, dataStr);
    pthread_mutex_unlock(&mutexData);
}

/**
 * Get the deceived data string
 * @return The received data string
 */
char* getReceivedData() 
{
    pthread_mutex_lock(&mutexData);
    if(receivedDataStatus == HAS_NEW_DATA)
	{
	    receivedDataStatus = NO_NEW_DATA;
	    pthread_mutex_unlock(&mutexData);
	    return receivedData;
	}
    pthread_mutex_unlock(&mutexData);
    return "";
}

/**
 * Set the status of running
 * @param status The status: RUN or STOP
 */
void setRunStatus(const int status)
{
    pthread_mutex_lock (&mutexStop);
    runStatus = status;
    pthread_mutex_unlock(&mutexStop);
}

/**
 * Get the status of running
 * @return The status: RUN or STOP
 */
const int getRunStatus()
{
    int cur_status = STOP;
    pthread_mutex_lock (&mutexStop);
    cur_status = runStatus;
    pthread_mutex_unlock(&mutexStop);
    return cur_status;
}

/**
 * Set the status of the recieved data
 * @param status HAS_NEW_DATA or NO_NEW_DATA
 */
void setReceivedDataStatus(const int status) 
{
    pthread_mutex_lock (&mutexData);
    receivedDataStatus = status;
    pthread_mutex_unlock(&mutexData);
}
