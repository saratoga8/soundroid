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


#ifndef PORTS_OPERATIONS_HEADER
#define PORTS_OPERATIONS_HEADER

#include <stdbool.h>


/**
 * Change the port of the daemon to the given port number
 * @param newPortNum The new port number
 * @return true - The change of the port has been successful
 */
bool  chgPortOfDaemon(const char *newPortNum);

/**
 * Get currently used port of daemon
 * @return The string of the used port number
 */
char* getPortOfDaemon();


/**
 * Get local IP by sending message with command to the server side
 * of the messages queue opened by the running
 * volume control daemon and getting a message
 * with IP from one.
 * @return The local IP string 
 */
char* getLocalIP();

/**
 * Get the IP of a connected client
 * by sending message with command to the server side
 * of the messages queue opened by the running
 * volume control daemon and getting a message
 * with IP from one.
 * @return The connected IP string 
 */
char* getConnectedIP();

#endif
