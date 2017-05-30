/**
 * @file
 * The dispatcher of commands using bluetooth for connecting to client
 * It dispatches commands between connectors, e.g. the command 'get local IP'
 * should be received from the GUI connector and sent to the network connector,
 * the result should be sent back to the GUI connector
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

#include "CommandsDispatcherBT.h"

#include "ConnectorBT.h"
#include "GuiConnector.h"
#include "SndConnector.h"

extern "C" {
#include "Log.h"
}
    
const char* CommandsDispatcherBT::TAG = "BT_COMMANDS_DISPATCHER";         /**< The tag for writting to log file */

/**
 * Constructor
 */
CommandsDispatcherBT::CommandsDispatcherBT() throw(GuiException)
{
    openLogFile(LOG_FILE_NAME);
    shouldStop_ = false;

    try
	{
	    initConnectors("");
	}
    catch (GuiException &e)
	{
	    stopConnectors();
	    delConnectors();
	    closeLogFile();
	    throw;
	}
    
    initThreads();
    initCommands();
    
    mutex_ = new mutex();
}

/**
 * Initialize threads instances
 */
void CommandsDispatcherBT::initThreads() 
{
    thNetConnector_  = new thread(&NetConnector::run, netConnector_);
    thGuiConnector_  = new thread(&GuiConnector::run, guiConnector_);
}

/**
 * Initialize connectors instances
 * @param portNum The port number string
 */
void CommandsDispatcherBT::initConnectors(const string &portNum) throw(GuiException)
{
	netConnector_ = new ConnectorBT();
	connectors_.push_back(netConnector_);
	
	guiConnector_ = new GuiConnector();
	connectors_.push_back(guiConnector_);

	sndConnector_ = new SndConnector();
	connectors_.push_back(sndConnector_);
}

/**
 * Restart the net connector with the given new port number
 * @param portNum The port number string (the new port)
 * @return true The connector has restarted successfully
 */
const bool CommandsDispatcherBT::restartNetConnector(const string &portNum)
{
    writeToLog("ERROR: the CommandsDispatcherBT.restartNetConnector() hasn't implemented!", TAG);
    return false;
}
