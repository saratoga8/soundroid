/**
 * @file
 * Connector to network
 * It opens port of daemon for connecting with clients by network
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
#include "ConnectorWiFi.h"

extern "C" {
#include "synchronise.h"
#include "SocketsLib.h"
#include "ClosingClient.h"
#include "Log.h"
#include "addr.h"
}

#include <sstream>
#include <iostream>

const char* ConnectorWiFi::TAG = "NET_CONNECTOR";         /**< The tag for writting to log file */

//string ConnectorWiFi::convertLong2Str(const long num)
//{
//	stringstream ss;
//	ss << num;
//	return ss.str();
//}

/**
 * Set the given port number
 * @param portNum The string with the port number
 */
void ConnectorWiFi::setPortNum(const string &portNum) throw(PortException)
{
    try
	{
	    stoul(portNum);
	}
    catch (std::invalid_argument &e)
	{
	    processPortNumErr(string("The given port number string '" + portNum + "' can't be converted to number\n"));
	}
    catch (std::out_of_range &e)
	{
	    processPortNumErr(string("The given port number string '" + portNum + "' is out of range\n"));
	}
    
    if (setPort(portNum.c_str()) == ERR)
	{
	    processPortNumErr(string("Can't set the port with number " + portNum + "\n"));
	}
    portNumStr_ = portNum;
}

/**
 * Process the string of the port number error
 * The error will be written to log file and thrown by exception
 * @param errStr The string of error
 */
void ConnectorWiFi::processPortNumErr(const string &errStr) const throw(PortException)
{
	writeToLog(errStr.c_str(), TAG);
	throw PortException(errStr);
}

/**
 * Stop the connector
 */
void ConnectorWiFi::stop()
{
	setRunStatus(STOP);
	runClosingClient(portNumStr_.c_str());
}

/**
 * Is the currently used port number available?
 * @return true The port is available
 */
const bool ConnectorWiFi::isPortAvailable()
{
	socketDescr = initConnectionBeforeListen();
	return (socketDescr != ERR);
}

/**
 * Run the connector
 */
void ConnectorWiFi::run()
{
	setRunStatus(RUN);
	runConnection(socketDescr);
	setRunStatus(STOP);
}

/**
 * Is the connector running?
 * @return true The connector is running
 */
bool ConnectorWiFi::isRunning() const
{
	return getRunStatus() == RUN;
}

/**
 * Get the last error string
 * @return The last error string
 */
const string ConnectorWiFi::getLastErrStr() const
{
	return getErrStr();
}

/**
 * Receive the data from client
 * @return The string of the received data
 */
const string ConnectorWiFi::receive()
{
    return getReceivedData();
}

/**
 * Send the given data string to client
 * @param dataStr The data string 
 */
void ConnectorWiFi::send(const string &dataStr)
{
	if(dataStr.empty())
	{
		writeToLog("WARNING: send(): can't send the given empty string", TAG);
		return;
	}
	setSentData(string(dataStr + "\n").c_str());
}

/**
 * Get the string of the local IP
 * @return The local IP string or "" if the IP hasn't found
 */
const string ConnectorWiFi::getLocalAddrStr() const
{
    const char* ip = getLocalAddr();
    if(ip == NULL)
    {
    	writeToLog("WARNING: getLocalIpStr(): The received local IP string is NULL\n", TAG);
    	ip = "";
    }
	return string(ip);
}

/**
 * Get the IP string of a connected client
 * @return The IP string of the connected client or "" if there is no one connected
 */
const string ConnectorWiFi::getConnectedAddrStr() const
{
    const char* ip = getConnectedAddr();
    if(ip == NULL)
    {
    	writeToLog("WARNING: getConnectedIpStr(): The received connected IP string is NULL\n", TAG);
    	ip = "";
    }
	return string(ip);
}

