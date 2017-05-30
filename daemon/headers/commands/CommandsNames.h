/**
 * The names of the used commands
 * @file
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

#ifndef COMMANDSNAMES_H_
#define COMMANDSNAMES_H_

#define COMMAND_STR_MAX_LEN 20           /**< The maximal length of a command name */

#define GET_PORT     "get_port"          /**< Get the currently used port number */
#define CHG_PORT     "chg_port"          /**< Change the currently used port number */ 
#define MUTE         "mute"              /**< Set mute for the volume of the system sound */
#define IS_MUTED     "is_muted"          /**< Is the system sound's volume muted */
#define UNMUTE       "unmute"            /**< Set unmute for the volume of the system sound */
#define LOCAL_IP     "get_local_ip"      /**< Get the local IP */
#define CONNECTED_IP "get_connected_ip"  /**< Get the IP of a connected client*/
#define QUIT         "quit"              /**< Quit */
#define CHG_VOL      "chg_vol"           /**< Change the current volume of the system sound */
#define GET_VOL      "get_vol"           /**< Get the current volume of the system sound */ 
#define HELLO        "hello"             /**< Hello */

#define ERR	     "ERR"               /**< Error - is the response to a command */
#define OK           "OK"                /**< OK - is the response to a command */
#define TRUE_        "true"              /**< true - is the response to a command */
#define FALSE_       "false"             /**< false - is the response to a command */


#endif
