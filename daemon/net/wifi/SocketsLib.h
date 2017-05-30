/**
 * @file
 * The library for connecting by sockets
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

#ifndef SOCKETS_LIB_H
#define SOCKETS_LIB_H

#define SOCKET_TIME_OUT 60   /**< Sockets connection time out in seconds */

#define ERR   -1             /**< an error's code */
#define NO_ERR 0             /**< no errors code  */

/**
 * Initial connection before listening
 * @return socket descriptor or ERR
 */
const int initConnectionBeforeListen();

/**
 * Run client-server connection
 * @param sockDescr The socket descriptor
 */
void runConnection(const int sockDescr);

/**
 * Set port number
 * @param port The port number string
 * @return ERR or NO_ERR
 */
const int setPort(const char* port);

/**
 * Get the string of the last error
 * @return The string of the last error
 */
const char* getErrStr();

#endif

