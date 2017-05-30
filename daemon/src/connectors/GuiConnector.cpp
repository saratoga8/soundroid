/**
 * Connector to GUI.
 * The connector lets send and receive commands to the GUI part
 * @file
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
#include "GuiConnector.h"
#include "CommandsNames.h"

#include <string>

#include <fstream>

extern "C" {
	#include <Log.h>
	#include "MsgsQueueServer.h"
}

using namespace std;

/**
 * Constructor
 */
GuiConnector::GuiConnector() throw(GuiException)
{
    if(!initQueueServer())
	throw GuiException();
    
    toStop_ = false;
}

/**
 * Run the connector
 * It runs the messages queue for sending and receiving
 * commands or data to GUI
 */
void GuiConnector::run()
{
    runQueue();
}

/**
 * Is the connector stopped?
 * @return true The connector is stopped
 */
bool GuiConnector::isStopped()
{
    bool result;
    mutex_.lock();
    result = toStop_;
    mutex_.unlock();
    return result;
}

/**
 * Stop the connector
 */
void GuiConnector::stop()
{
    mutex_.lock();
    toStop_ = true;
    mutex_.unlock();
    
    deleteQueue();
}

/**
 * Send the given data string to GUI
 * @param newDataStr The data string for sending
 */
void GuiConnector::send(const string &newDataStr)
{
    const char* msgTxt = (newDataStr.length() == 0) ? "None": newDataStr.c_str();
    sendMsgServer(msgTxt);
}

/**
 * Receive a data string from GUI
 * @return The data string from GUI
 */
const string GuiConnector::receive()
{
    return receiveMsgServer();
}
