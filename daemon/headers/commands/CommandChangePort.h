/**
 * Command for changing port
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

#ifndef COMMAND_CHG_PORT
#define COMMAND_CHG_PORT

#include "Command.h"
#include "CommandsDispatcher.h"

#include <iostream>
#include <stdexcept>

using namespace std;

/**
 * Command for changing port
 */
class CommandChangePort : public Command 
{
    static const char* TAG;                   /**< The tag for writing to log file */ 

    CommandsDispatcher &dispatcher_;          /**< Commands dispatcher */

    /**
     * Constructor
     */
    CommandChangePort() = delete;

    /**
     * Execute command without parameters
     * @return The string of execution result
     */
    string execute() { std::cerr << "The command hasn't executed\n"; return ERR; };

 public:
    /**
     * Constructor
     * @param dispatcher The dispatcher reference for initializing the local reference to the dispatcher
     */
    CommandChangePort(CommandsDispatcher &dispatcher) throw (invalid_argument&): dispatcher_(dispatcher) {}

    /**
     * Execute command with the given list of parameters
     * @param params The list of the parameters'' strings
     * @return The string of execution result     
     */
    string execute(const list<string> &params) const;
};

#endif
