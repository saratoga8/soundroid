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

#include "CommandChangePort.h"

extern "C" {
#include "Log.h"
}

using namespace std;

const char* CommandChangePort::TAG = "COMMAND_CHG_PORT";   /**< The tag for writing to log file */ 

/**
 * Execute the command for changing port with the given parameters
 * @param params The parameters of the command
 * @return The string of the commands result: ERR or OK
 */
string CommandChangePort::execute(const list<string> &params) const
{
    if(params.size() == 0)
	{
	    writeToLog(string("ERROR: execute(): The given list of parameters is empty").c_str(), TAG);
	    return ERR;
	}
    string param = params.front();
    if(param.empty())
	{
	    writeToLog(string("ERROR: execute(): The given parameter is empty").c_str(), TAG);
	    return ERR;
	}
    const string portStr = params.front();
    
    return (dispatcher_.restartNetConnector(portStr)) ? OK: ERR;
}

