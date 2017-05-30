/**
 * @file
 * The abstract class of connector
 * The target of the connector is to receive a data
 * and to send one.
 * E.g. network connector is for connecting data between
 * command dispatcher and network. When the dispatcher
 * wants to receive a data from network it calls to the
 * function receive() of the connector. If the dispatcher
 * wants to send a data to network it calls the function
 * send() of the connector.
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

#ifndef CONNECTOR_H_
#define CONNECTOR_H_

#include <string>


/**
 * The target of the connector to connect some item(GUI, sound system or network)
 * with the daemon.
 * The connector can send and receive a string data.
 */
class Connector
{
public:

    /**
     * Destructor
     */
    virtual ~Connector() {}
    
    /**
     * Send data string to connector
     * @param dataStr The string of the sent data
     */
    virtual void send(const std::string &dataStr) = 0;
    
    /**
     * Get a data string arrived to the connector
     * @return The arrived data string
     */
    virtual const std::string receive() = 0;

    /**
     * Stop the connector
     */
    virtual void stop() = 0;

    /**
     * Run the connector
     */
    virtual void run() = 0;
};


#endif 
