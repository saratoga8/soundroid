/**
 * @file
 * Bluetooth connector
 * It runs Bluetooth SDP service for connecting with clients by bluetooth
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
#include "ConnectorBT.h"

extern "C" {
#include "synchronise.h"
#include "BlueToothLib.h"
#include "Log.h"
#include "addr.h"
#include "dlfcn.h"
}

#include <sstream>
#include <iostream>

const char* ConnectorBT::TAG = "BT_CONNECTOR";         /**< The tag for writting to log file */

#define BT_DYN_LIB "libBlueTooth.so"                   /**< The name of the dynamic library of bluetooth connection to daemon */

/**
 * Constructor
 */
ConnectorBT::ConnectorBT()
{
    socketDescr = initBtConnectionBeforeListen();
}

/**
 * Stop the connector
 */
void ConnectorBT::stop()
{
    setRunStatus(STOP);
}

/**
 * Run the connector
 */
void ConnectorBT::run()
{
    setRunStatus(RUN);
    runBtConnection(socketDescr);
    setRunStatus(STOP);
}

/**
 * Is the connector running?
 * @return true The connector is running
 */
bool ConnectorBT::isRunning() const
{
    return getRunStatus() == RUN;
}

/**
 * Get the last error string
 * @return The last error string
 */
const string ConnectorBT::getLastErrStr() const
{
    return "";
}

/**
 * Receive the data from client
 * @return The string of the received data
 */
const string ConnectorBT::receive()
{
    return getReceivedData();
}

/**
 * Send the given data string to client
 * @param dataStr The data string 
 */
void ConnectorBT::send(const string &dataStr)
{
	if(dataStr.empty())
	{
		writeToLog("WARNING: send(): can't send the given empty string", TAG);
		return;
	}
	setSentData(string(dataStr + "\n").c_str());
}

/**
 * Call to the function from a dynamic library
 * @param funcName The name of the function
 * @param libName The name of the library
 * @return The string result of the function
 */
const char* ConnectorBT::callToFuncFromDynLib(const char *funcName, const char *libName) const
{
    void *handle = dlopen(libName, RTLD_LAZY);
    if(!handle)
	{
	    writeToLog2("ERROR: Can't load ", libName, TAG);
	    writeToLog2("\t", dlerror(), TAG);
	    return "";
	}
    dlerror();

    const char* (*func)() = (const char* (*)())dlsym(handle, funcName);
    char *err = dlerror();
    if(err != NULL)
	{
	    writeToLog2("ERROR: Can't get function from the ", libName, TAG);
	    writeToLog2("\t", err, TAG);
	    return "";
	}

    const char* returnedStr = (*func)();
    
    if(dlclose(handle) != 0)
	{
	    writeToLog("ERROR: Can't close the handle of the opened dynamic library\n", TAG);
	}
    return returnedStr;
}

/**
 * Get the string of the local address
 * @return The local address string or "" if the address hasn't found
 */
const string ConnectorBT::getLocalAddrStr() const
{
    const char *addr = callToFuncFromDynLib("getLocalAddr", BT_DYN_LIB);
    if(addr == NULL)
    {
    	writeToLog("WARNING: getLocalAddrStr(): The received local address string is NULL\n", TAG);
    	addr = "";
    }
    return string(addr);
}

/**
 * Get the IP string of a connected client
 * @return The IP string of the connected client or "" if there is no one connected
 */
const string ConnectorBT::getConnectedAddrStr() const
{
    const char* addr = callToFuncFromDynLib("getConnectedAddr", BT_DYN_LIB);
    if(addr == NULL)
    {
    	writeToLog("WARNING: getConnectedAddrStr(): The received connected address string is NULL\n", TAG);
    	addr = "";
    }
	return string(addr);
}


/**
 * Get the string of the currently used port number
 * @return The string of the port number
 */
const string ConnectorBT::getUsedPort() const
{
    writeToLog("WARNING: the func getUsedPort() can't be used'", TAG);
    return "";
}

/**
 * Set the port number
 * @param portNum The string of the new port number string
 */
void ConnectorBT::setPortNum(const string &portNum) throw(PortException)
{
    throw PortException("the func CommandsDispatcherBT::setPortNum() can't be used");
}

/**
 * Is the currently used port available
 * @return true The port available
 */
const bool ConnectorBT::isPortAvailable()
{
    writeToLog("WARNING: the func isPortAvailable() can't be used'", TAG);
    return false;
}
