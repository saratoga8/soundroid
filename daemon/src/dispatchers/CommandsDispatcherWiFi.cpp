/**
 * @file
 * The dispatcher of commands using wifi for connecting to client
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
#include "CommandsDispatcherWiFi.h"

#include "CommandChangePort.h"
#include "CommandGetPort.h"

extern "C" {
#include "Log.h"
}

const char* CommandsDispatcherWiFi::TAG = "WIFI_COMMANDS_DISPATCHER";         /**< The tag for writting to log file */

/**
 * Constructor
 * @param portNum The number of port string
 */
CommandsDispatcherWiFi::CommandsDispatcherWiFi(string &portNum) throw(PortException, GuiException)
{
    openLogFile(LOG_FILE_NAME);
    shouldStop_ = false;

    try
	{
	    initConnectors(portNum);
	    initThreads();
	}
    catch (PortException &e)
	{
	    stopConnectors();
	    //	    delConnectors();
	    //	    closeLogFile();
	    throw;
	}
    catch (GuiException &e)
	{
	    stopConnectors();
	    //	    delConnectors();
	    //	    closeLogFile();
	    throw;
	}
    
    initCommands();
    mutex_ = new mutex();
}

/**
 * Change the port number of the net connector
 * @param portNum The port number string (the new port)
 * @param prevPortNum The previous port number (the current port)
 * @return true The port has changed successfully
 */
const bool CommandsDispatcherWiFi::chgNetConnectorPort(const string &portNum, const string &prevPortNum)
{
    netConnector_->setPortNum(portNum);
    const bool result = netConnector_->isPortAvailable();
    if(!result)
	{
	    netConnector_->setPortNum(prevPortNum);
	    netConnector_->isPortAvailable();
	}
    return result;
}

/**
 * Restart the net connector with the given new port number
 * @param portNum The port number string (the new port)
 * @return true The connector has restarted successfully
 */
const bool CommandsDispatcherWiFi::restartNetConnector(const string &portNum)
{
	if(thNetConnector_ == NULL)
	{
		writeToLog("WARNING: restartNetConnector(): the net connector instance is NULL", TAG);
		return false;
	}
	if(portNum.empty())
	{
		writeToLog("WARNING: restartNetConnector(): the given port number string is empty", TAG);
		return false;
	}

	string prevPortNum = netConnector_->getUsedPort();

	netConnector_->stop();

	thNetConnector_->join();
	delete thNetConnector_;

	const bool portChanged = chgNetConnectorPort(portNum, prevPortNum);

	thNetConnector_  = new thread(&NetConnector::run, netConnector_);
	return portChanged;
}

/**
 * Initialize threads instances
 */
void CommandsDispatcherWiFi::initThreads() throw(PortException)
{
	if(!netConnector_->isPortAvailable())
	{
		throw PortException(netConnector_->getLastErrStr());
	}
	thNetConnector_  = new thread(&NetConnector::run, netConnector_);
	thGuiConnector_  = new thread(&GuiConnector::run, guiConnector_);
}

/**
 * Initialize connectors instances
 * @param portNum The port number string
 */
void CommandsDispatcherWiFi::initConnectors(const string &portNum) throw(PortException, GuiException)
{
	netConnector_ = new ConnectorWiFi(portNum);
	connectors_.push_back(netConnector_);
	
	guiConnector_ = new GuiConnector();
	connectors_.push_back(guiConnector_);

	sndConnector_ = new SndConnector();
	connectors_.push_back(sndConnector_);
}

/**
 * Initialize commands instances
 */
void CommandsDispatcherWiFi::initCommands()
{
    CommandsDispatcher::initCommands();
    commands_[GET_PORT]     = new CommandGetPort(*netConnector_);
    commands_[CHG_PORT]     = new CommandChangePort(*this);
}
