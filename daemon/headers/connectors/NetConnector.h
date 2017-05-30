/**
 * @file
 * The abstract class of the network connector
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

#ifndef NET_CONNECTOR_H_
#define NET_CONNECTOR_H_

#include "Connector.h"
#include "PortException.h"

#include <string>

using namespace std;

/**
 * The abstract class of the network connector
 */
class NetConnector: public Connector
{
    
 public:
    /**
     * Get the string of the local address
     * @return The string of the local address
     */
    virtual const string getLocalAddrStr() const = 0;

    /**
     * Get the string of the connected address
     * @return The string of the address
     */
    virtual const string getConnectedAddrStr() const = 0;

    /**
     * Get the string of the currently used port number
     * @return The string of the port number
     */
    virtual const string getUsedPort() const = 0;

    /**
     * Set the port number
     * @param portNum The string of the new port number string
     */
    virtual void setPortNum(const string &portNum) throw(PortException) = 0;

    /**
     * Is the currently used port available
     * @return true The port available
     */
    virtual const bool isPortAvailable() = 0;

    /**
     * Get the last error string
     * @return The string of the last error
     */
    virtual const string getLastErrStr() const = 0;
};

#endif 
