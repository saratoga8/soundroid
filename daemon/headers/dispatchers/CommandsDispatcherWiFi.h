/**
 * @file
 * The dispatcher of commands sent by wifi
 * It dispatches commands between connectors by WiFi, e.g. the command 'get local IP'
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

#ifndef COMMANDSDISPATCHER_WIFI_H_
#define COMMANDSDISPATCHER_WIFI_H_


#include "CommandsDispatcher.h"
#include "PortException.h"
#include "GuiException.h"

/**
 * The dispatcher of commands sent by wifi
 */
class CommandsDispatcherWiFi: public CommandsDispatcher 
{
    static const char* TAG;   /**< The for writing to log file */
    
    /**
     * Change the port number of the net connector
     * @param portNum The port number string (the new port)
     * @param prevPortNum The previous port number (the current port)
     * @return true The port has changed successfully
     */
    const bool chgNetConnectorPort(const string &portNum, const string &prevPortNum);

    /**
     * Initialize threads instances
     */
    void initThreads() throw(PortException);

    /**
     * Initialize connectors instances
     * @param portNum The port number string
     */
    void initConnectors(const string &portNum) throw(PortException, GuiException);

    /**
     * Initialize commands instances
     */
    void initCommands();

    /**
     * Constructor
     */
    CommandsDispatcherWiFi() {}
    
 public:
    /**
     * Constructor
     * @param portNum The number of port string
     */
    CommandsDispatcherWiFi(string &portNum) throw(PortException, GuiException);
        
    /**
     * Restart the net connector with the given new port number
     * @param portNum The port number string (the new port)
     * @return true The connector has restarted successfully
     */
    const bool restartNetConnector(const string &portNum);
};

#endif
