/**
 * @file
 * The dispatcher of commands.
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

#ifndef COMMANDSDISPATCHER_H_
#define COMMANDSDISPATCHER_H_

#include <map>
#include <string>
#include <mutex>
#include <thread>
#include <list>

#include "Command.h"

#include "GuiConnector.h"
#include "NetConnector.h"
#include "SndConnector.h"

/**
 * \class CommandsDispatcher
 * \brief Contains the commands set. Dispatches the commands between connectors
 */
class CommandsDispatcher {

	static const char* TAG;            /**< The for writing to log file */

 protected:

	map<string, Command*> commands_;   /**< The map of commands */

	NetConnector *netConnector_;       /**< The connector for network */
	GuiConnector *guiConnector_;       /**< The connector for GUI */
	SndConnector *sndConnector_;       /**< The connector for system sound control */

	bool shouldStop_;                  /**< Should the dispatcher be stopped */
	mutex *mutex_;                     

	thread *thNetConnector_;           /**< The thread of the network connector */
	thread *thGuiConnector_;           /**< The thread of the GUI connector */

	list<Connector*> connectors_;      /**< The list of connectors */

	/**
	 * Initialize commands instances
	 */
	void initCommands();
	
	/**
	 * Delete commands collection
	 */
	void delCommands();
	
	/**
	 * Execute the command by given string of the command
	 * @param command The command's string containing the command's name and parameters
	 * @return The execution result string, "ERR" or "OK"
	 */
	const string execCommand(const string& command) const;

	/**
	 * Initialize connectors instances
	 * @param portNum The port number string
	 */
	virtual void initConnectors(const string &portNum) = 0;
	
	/**
	 * Delete connectors
	 */
	void delConnectors();
	
	/**
	 * Initialize threads instances
	 */
	virtual void initThreads() = 0;
	
	/**
	 * Delete threads
	 */
	void delThreads();

	/**
	 * Parse the given command string into command name and parameters if there are ones
	 * @param str The string of the command
	 * @return The list of the name of the command and its parameters
	 */
	const list<string> parseCommandStr(const string &str) const;

	/**
	 * Stop connectors
	 */	
	void stopConnectors() const;

 public:

	/**
	 * Destructor
	 */
	virtual ~CommandsDispatcher();

	/**
	 * Start the dispatcher
	 */	
	void start();

	/**
	 * Stop the dispatcher
	 */
	void stop();

	/**
	 * Has the dispatcher stopped?
	 * @return true - The dispatcher has stopped
	 */	
	bool isStopped();

	/**
	 * Restart the net connector with the given new port number
	 * @param portNum The port number string (the new port)
	 * @return true The connector has restarted successfully
	 */
	virtual const bool restartNetConnector(const string &portNum) = 0;
};

#endif
