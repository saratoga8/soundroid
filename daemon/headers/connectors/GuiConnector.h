/**
 * @file
 * Connector between daemon and its GUI
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

#ifndef GUI_CONNECTOR_H_
#define GUI_CONNECTOR_H_

#include <mutex>
#include <string>
#include "Connector.h"
#include <thread>
#include "GuiException.h"

/**
 * The class for connecting the daemon to its GUI
 */
class GuiConnector: public Connector
{    
    constexpr static const char* TAG = "GUI_CONNECTOR";                /**< The tag for writing to the log file */
    
    bool toStop_;                                                      /**< Should the connector be stopped? */
    std::mutex mutex_;                                                

    /**
     * Is the connector stopped?
     * @return true The connector has been stopped
     */
    bool isStopped();                   

public:
    /**
     * Constructor
     */
    GuiConnector() throw(GuiException);

    /**
     * Destructor
     */
    ~GuiConnector() {}

    /**
     * Run the connector
     */
    void run();

    /**
     * Stop the connector
     */    
    void stop();

    /**
     * Send data string to connector
     * @param newDataStr The string of the sent data
     */    
    void send(const std::string &newDataStr);

    /**
     * Get a data string arrived to the connector
     * @return The arrived data string
     */    
    const std::string receive();
};

#endif
