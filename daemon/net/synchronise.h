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


#ifndef __SYNCHRONISE_H
#define __SYNCHRONISE_H

#include <pthread.h>

#define RUN  2                             /**< The connection is running */
#define STOP 3                             /**< The connection is stopped */

#define NO_NEW_DATA  0                     /**< No new data received */
#define HAS_NEW_DATA 1                     /**< The new data arrived */

#define DATA_LEN 50                        /**< The data length */                        

#define MILLISECONDS_SLEEP_TIME 500        /**< The sleep time in milli seconds */

/**
 * Destroy mutexes
 */
void destroyMutexes();

/**
 * Initialize mutexes
 */
void initMutexes();

/**
 * Get pointer to the sent data string
 * @return The string of the sent data
 */
char* getSentData();

/**
 * Set data string for sending
 * @param dataStr data string
 */
void setSentData(const char *dataStr);

/**
 * Get the received data string
 * @return The received data string
 */
char* getReceivedData();

/**
 * Set received data string by the given string
 * @param dataStr The string for setting received data
 */
void setReceivedData(const char *dataStr);

/**
 * Set the status of running
 * @param status The status: RUN or STOP
 */
void setRunStatus(const int status);

/**
 * Get the status of running
 * @return The status: RUN or STOP
 */
const int getRunStatus();

/**
 * Set the status of the recieved data
 * @param status HAS_NEW_DATA or NO_NEW_DATA
 */
void setReceivedDataStatus(const int status);

#endif
