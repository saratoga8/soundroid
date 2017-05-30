/**
 * Command to mute the current volume of the system sound
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


#ifndef COMMAND_MUTE
#define COMMAND_MUTE

#include "Command.h"
#include <iostream>
#include "SndConnector.h"

/**
 * Command to mute the current volume of the system sound
 */
class CommandMute : public Command 
{
    SndConnector &sndConnector_;              /**< The sound connector's reference */
    
    CommandMute() = delete;

 public:
    /**
     * Constructor
     * @param sndConnector The reference for initializing of the local sound connector's reference
     */
     CommandMute(SndConnector &sndConnector): sndConnector_(sndConnector) {}
    
    /**
     * Destructor
     */
    ~CommandMute() {}

     /**
     * Execute command without parameters
     * @return The string of execution result
     */    
    std::string execute();

    /**
     * Execute command with the given list of parameters
     * @param params The list of the parameters'' strings
     * @return The string of execution result     
     */        
    std::string execute(const std::list<std::string> &params) const { std::cerr << "The command mute with parameters hasn't implemented\n";
        							      return OK; };
};

#endif
