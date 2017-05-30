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

#include "CommandsNames.h"
#include "CommandsDispatcher.h"


#include "CommandMute.h"
#include "CommandIsMuted.h"
#include "CommandUnMute.h"
#include "CommandQuit.h"
#include "CommandGetConnectedIP.h"
#include "CommandGetLocalIP.h"
#include "CommandChgVol.h"
#include "CommandHello.h"
#include "CommandGetCurVol.h"


#include <algorithm>
#include <thread>
#include <list>
#include <sstream>

extern "C" {
	#include "Log.h"
	#include <sys/msg.h>
}

const char* CommandsDispatcher::TAG = "COMMANDS_DISPATCHER";   /**< The for writing to log file */

/**
 * Initialize commands instances
 */
void CommandsDispatcher::initCommands()
{
	commands_ = map<string, Command*>();

	commands_[HELLO]        = new CommandHello(*netConnector_);
	commands_[LOCAL_IP]     = new CommandGetLocalIP(*netConnector_);
	commands_[CONNECTED_IP] = new CommandGetConnectedIP(*netConnector_);
	commands_[IS_MUTED]     = new CommandIsMuted(*sndConnector_);
	commands_[MUTE]         = new CommandMute(*sndConnector_);
	commands_[UNMUTE]       = new CommandUnMute(*sndConnector_);
	commands_[CHG_VOL]      = new CommandChgVol(*sndConnector_);
	commands_[GET_VOL]      = new CommandGetCurVol(*sndConnector_);
	commands_[QUIT]         = new CommandQuit(*this);
}

/**
 * Delete commands collection
 */
void CommandsDispatcher::delCommands()
{
    for( auto it=commands_.begin(); it!=commands_.end(); ++it)
	{
	    delete it->second;
	}
}

/**
 * Destructor
 */
CommandsDispatcher::~CommandsDispatcher()
{
    delCommands();
    delete mutex_;    
    delThreads();
    delConnectors();
    
    closeLogFile();
}

/**
 * Delete threads
 */
void CommandsDispatcher::delThreads()
{
    if(thGuiConnector_ != NULL)
	{
	    thGuiConnector_->join();
	    delete thGuiConnector_;
	}

    if(thNetConnector_ != NULL)
	{
	    thNetConnector_->join();
	    delete thNetConnector_;
	}
}

/**
 * Delete connectors
 */
void CommandsDispatcher::delConnectors()
{
	for(auto it = connectors_.begin(); it != connectors_.end(); ++it)
		delete  *it;
	connectors_.clear();
}

/**
 * Stop connectors
 */
void CommandsDispatcher::stopConnectors() const
{
    for(auto it = connectors_.begin(); it != connectors_.end(); ++it)
	(*it)->stop();
}

/**
 * Parse the given command string into command name and parameters if there are ones
 * @param str The string of the command
 * @return The list of the name of the command and its parameters
 */
const list<string> CommandsDispatcher::parseCommandStr(const string &str) const
{
	istringstream iss(str, istringstream::in);
	list<string> words;
	string word;
	while(iss >> word)
		words.push_back(word);
	return words;
}

/**
 * Execute the command by given string of the command
 * @param command The command's string containing the command's name and parameters
 * @return The execution result string, "ERR" or "OK"
 */
const string CommandsDispatcher::execCommand(const string& command) const
{
	if(command.empty())
	{
		writeToLog(string("ERROR: execCommand(): The given command is empty\n").c_str(), TAG);
		return ERR;
	}

	list<string> commandList = parseCommandStr(command);  // list: {command_name, parameter, parameter, ...}
	if(commandList.empty())
	{
		writeToLog(string("ERROR: execCommand(): Can't parse the given command '" + command + "'\n").c_str(), TAG);
		return ERR;
	}

	auto commandIt = commandList.begin();
	string name = *commandIt;

	map<string, Command*>::const_iterator it = commands_.find(name);
    if(it == commands_.end())
    {
	writeToLog(string("ERROR: execCommand(): The command '" + command + "' doesn't exist\n").c_str(), TAG);
        return ERR;
    }

    if(commandList.size() == 1)   // command without params
    	return (*it).second->execute();

    commandList.pop_front();      // drop the command's name
    return (*it).second->execute(commandList);
}

/**
 * Stop the dispatcher
 */
void CommandsDispatcher::stop()
{
	mutex_->lock();
	shouldStop_ = true;
	mutex_->unlock();
}

/**
 * Has the dispatcher stopped?
 * @return true - The dispatcher has stopped
 */
bool CommandsDispatcher::isStopped()
{
	bool status;

	mutex_->lock();
	status = shouldStop_;
	mutex_->unlock();
	return status;
}

/**
 * Start the dispatcher
 */
void CommandsDispatcher::start()
{
    while(!isStopped())
	{
	    string newCommand;
	    for(Connector* connector: connectors_)
		{
		    newCommand = connector->receive();
		    if(!newCommand.empty())
			{
			    const string res = execCommand(newCommand);
			    connector->send(res);
			}			
		    std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}

	stopConnectors();
	writeToLog("The commands dispatcher has stopped\n", TAG);
}
