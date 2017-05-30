/**
 * The command for executing command 'quit'
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

#ifndef COMMANDQUIT_H_
#define COMMANDQUIT_H_

#include "Command.h"
#include <iostream>
#include "CommandsDispatcher.h"

/**
 * The command for executing command 'quit'
 */
class CommandQuit: public Command
{
    CommandsDispatcher &dispatcher_;   /**< The reference to the commands dispatcher */
    
    CommandQuit() = delete;
    
    static const char* TAG;         /**< The tag for writing to log file */

 public:
    /**
     * Constructor
     * @param dispatcher The reference to dispatcher for initializing the local reference to one
     */
    CommandQuit(CommandsDispatcher &dispatcher): dispatcher_(dispatcher) {}

    /**
     * Destructor
     */
    ~CommandQuit() {}

    /**
     * Execute command without parameters
     * @return The string of execution result
     */    
    std::string execute() { dispatcher_.stop();
	                    return OK; }
    /**
     * Execute command with the given list of parameters
     * @param params The list of the parameters'' strings
     * @return The string of execution result     
     */
    std::string execute(const std::list<std::string> &params) const { std::cerr << "The command mute with parameters hasn't implemented\n";
    								      return OK; };
};


#endif
