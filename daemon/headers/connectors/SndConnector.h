/**
 * @file
 * Connector between the system's sound and the daemon
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

#ifndef SNDCONNECTOR_H_
#define SNDCONNECTOR_H_

#include "Connector.h"

#include <mutex>
#include <string>

/**
 * Class for changing sound volume and its state(mute/unmute)
 */
class SndConnector: public Connector
{
    std::mutex mutex_;
    
    std::string dataStr_;                    /**< The string of a data */

    static const char* TAG;                  /**< The tag for writing to the log file */

    /**
     * Set the string of an arrived data
     * @param str The string of the arrived data
     */
    void setArrivedDataStr(const std::string &str);

 public:
    /**
     * Constructor
     */
    SndConnector(): dataStr_("") {}

    /**
     * Destructor
     */
    ~SndConnector() {}

    /**
     * Run the connector
     */    
    void run() {}

    /**
     * Stop the connector
     */    
    void stop() {}

    /**
     * Send data string to connector
     * @param newDataStr The string of the sent data
     */
    void send(const std::string &newDataStr) {}

    /**
     * Get a data string arrived to the connector
     * @return The arrived data string
     */    
    const std::string receive();

    /**
     * Make the system sound state muted
     */
    void doMute();

    /**
     * Make the system sound state unmuted
     */    
    void doUnmute();

    /**
     * Change system's sound volume
     * @param value The value which the volume should be changed to
     */
    void doChgVol(const int value);

    /**
     * Get the sting of the current system's sound volume value
     * @return The string of the value
     */
    const std::string doGetVol();

    /**
     * Is the system's sound muted?
     * @return true The system's sound muted
     */
    const std::string doIsMuted();
};

#endif
