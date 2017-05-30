/**
 * @file
 * The connector between the daemon and the client by bluetooth
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

#ifndef BT_CONNECTOR_H
#define BT_CONNECTOR_H

#include <string>
#include <thread>
#include <mutex>

#include "NetConnector.h"

using namespace std;

/**
 * The class should send, receive string data between the daemon and a connected clint,
 * make port operations
 */
class ConnectorBT: public NetConnector
{
    static const char* TAG;                   /**< The tag for writing to the log file */

    mutex mutex_;
    
    int socketDescr;                          /**< The currently used socket descriptor */

    /**
     * Call to the function from a dynamic library
     * @param funcName The name of the function
     * @param libName The name of the library
     * @return The string result of the function
     */
    const char* callToFuncFromDynLib(const char *funcName, const char *libName) const;

 public:

    /**
     * Constructor
     */
    ConnectorBT();
    
    /**
     * Destructor
     */
    ~ConnectorBT() {}

    /**
     * Run the connector
     */
    void run();

    /**
     * Is the connector running?
     * @return true The connector is running
     */
    bool isRunning() const;

    /**
     * Send data string to connector
     * @param dataStr The string of the sent data
     */    
    void send(const string &dataStr);

    /**
     * Get a data string arrived to the connector
     * @return The arrived data string
     */    
    const string receive();

    /**
     * Stop the connector
     */    
    void stop();

    /**
     * Get the string of the local address
     * @return The string of the local address
     */
    const string getLocalAddrStr()     const;

    /**
     * Get the string of the connected address
     * @return The string of the address
     */
    const string getConnectedAddrStr() const;

    /**
     * Get the last error string
     * @return The string of the last error
     */
    const string getLastErrStr() const;

    /**
     * Get the string of the currently used port number
     * @return The string of the port number
     */
    const string getUsedPort() const;

    /**
     * Set the port number
     * @param portNum The string of the new port number string
     */
    void setPortNum(const string &portNum) throw(PortException);

    /**
     * Is the currently used port available
     * @return true The port available
     */
    const bool isPortAvailable();    
};

#endif
