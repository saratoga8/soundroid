/**
 * @file
 * The Linux daemon for controlling sound volume
 * The daemon runs the thread with Commands Dispatcher(dispatching commands from between GUI, network and ALSA)
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

extern "C" {
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
}

#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>

#include "CommandsDispatcher.h"
#include "CommandsDispatcherWiFi.h"
#include "CommandsDispatcherBT.h"
#include "ConnectionTypes.h"

/**<
   \def FILE_NAME
   \brief The file with the the process ID
*/
#define FILE_NAME "pid.txt"   

bool stop = false;            /**< Is the process stopped */

//#define SIGNAL SIGUSR1
#define ERROR -1              /**< an error code */

/**
 * The handler function of the given signal
 * @param signo The signal number
 */
void sig_handler(int signo)
{
    stop = (signo == SIGTERM);
}

/**
 * Kill the other daemon process if it's running
 *
void killOtherProcess()
{
    ifstream inFile(FILE_NAME, fstream::in);
    
    string pid_str;
    inFile >> pid_str;
    
    if(!pid_str.empty())
	{
	    istringstream str_stream(pid_str);
	    int pid;
	    str_stream >> pid;
	    cout << "Killing the existing process PID=" << pid << endl;
	    if(kill(pid, SIGUSR1) == ERROR)
		{
		    cerr << "ERROR: Can't send signal to the process with PID=" << pid_str << ". " << strerror(errno) << endl;
		}
	}

    inFile.close();
}*/

/**
 * Save the ID of the process to file name FILE_NAME
 */
void savePidToFile()
{
    ofstream outFile(FILE_NAME, fstream::out);
    
    ostringstream str_stream;
    str_stream << getpid();
    outFile << str_stream.str();

    try
	{
	    outFile.close();
	}
    catch (ofstream::failure e)
	{
	    cerr << "ERROR: can't close the file " + string(FILE_NAME_OF_LAST_USED_CONNECTION_TYPE) + ": " + string(e.what()) << endl;
	}
}

/**
 * Close the standard streams stdin, stdout. Except stderr
 */
void closeStandardStreams()
{
    if(close(STDIN_FILENO) == ERROR)
	cerr << "ERROR: can't close the standard input stream: " << strerror(errno) << endl;
    if(close(STDOUT_FILENO) == ERROR)
	cerr << "ERROR: can't close the standard output stream: " << strerror(errno) << endl;
    //    close(STDERR_FILENO);
}

/**
 * Spawn a new process
 */
void spawn()
{
    pid_t pid = fork();
    if (pid < 0)
	exit(EXIT_FAILURE);
    if (pid > 0)
	exit(EXIT_SUCCESS);
    
    umask(0);
    
    pid_t sid = setsid();
    if (sid < 0)
	{
	    cout << "ERROR: Could not create process group\n";
	    exit(EXIT_FAILURE);
	}    
}

/**
 * Run the given commands dispatcher instance
 * @param dispatcher The instance of the dispatcher
 */
int runDispatcher(CommandsDispatcher *dispatcher)
{
    int exit_status = EXIT_FAILURE;
    try
    {
	savePidToFile();
	
        if(signal(SIGTERM, sig_handler) == SIG_ERR)
	    {
		delete dispatcher;
		return exit_status;
	    }    	    

        thread dispatchingThread(&CommandsDispatcher::start, dispatcher);

        while(!stop)
	    sleep(1);

        dispatcher->stop();
        dispatchingThread.join();

	exit_status = EXIT_SUCCESS;
    }
    catch (PortException &e)
    {
    	cerr << string("ERROR: ") + string(e.what()) << endl;
    }
    
    delete dispatcher;
    return exit_status;
}

/**
 * Print help message to the given stream
 * @param out The output stream for printing
 * @param progName The name of this program 
 * @param firstLine The first line of the help message
 */
void printHelp(std::ostream &out, const char* progName, const string &firstLine = string(""))
{
    if(firstLine != string(""))
	out << firstLine;
    out << "\tFor running with Bluetooth connection: '" << progName << " bt'\n";
    out << "\tFor running with WiFi      connection: '" << progName << " wifi PORT_NUMBER'\n";
}

/**
 * Build a command dispatcher using bluetooth for connection to a client
 * @param paramsArr The array of parameters from the command line
 * @return The instance of command dispatcher or NULL
 */
CommandsDispatcherBT* getCommandsDispatcherBT(char* paramsArr[])
{
    try
	{
	    if(paramsArr[1] == string("-h"))
		printHelp(cout, paramsArr[0]);
	    else if(paramsArr[1] == string("bt"))
		return new CommandsDispatcherBT();
	    else
		printHelp(cerr, paramsArr[0], "ERROR: Unknown parameter\n");
	}
    catch (GuiException &e) 
	{
	    cerr << string("ERROR: can't initialise commands bluetooth dispatcher: ") + string(e.what()) << endl;
	}    
    return NULL;
}

/**
 * Build a command dispatcher using WiFi for connection to a client
 * @param paramsArr The array of parameters from the command line
 * @return The instance of command dispatcher or NULL
 */
CommandsDispatcherWiFi* getCommandsDispatcherWiFi(char* paramsArr[])
{
    try
	{
	    cout << paramsArr[1] << endl;
	    if(paramsArr[1] == string("wifi"))
		{
		    string port = paramsArr[2];
		    return new CommandsDispatcherWiFi(port);
		}
	    else
		printHelp(cerr, paramsArr[0], "ERROR: Unknown parameter\n");	    
	}
    catch (PortException &e) 
	{
	    cerr << string("ERROR: can't initialise commands WiFi dispatcher: ") + string(e.what()) << endl;
	}
    catch (GuiException &e) 
	{
	    cerr << string("ERROR: can't initialise commands WiFi dispatcher: ") + string(e.what()) << endl;
	}    
    return NULL;
}

/**
 * Save the connection type to the file
 * @param connType The connection type number
 */
void saveConnectionType(const int connType)
{
    ofstream os(FILE_NAME_OF_LAST_USED_CONNECTION_TYPE, ios::in | ios::trunc);

    os << connType;
    
    try
	{
	    os.close();
	}
    catch (ofstream::failure e)
	{
	    cerr << "ERROR: can't close the file " + string(FILE_NAME_OF_LAST_USED_CONNECTION_TYPE) + ": " + string(e.what()) << endl;
	}
}

/**
 * Get commands dispatcher which type depends on the given parameters from 
 * the command line
 * @param paramsNum The number of parameters from the command line
 * @param paramsArr The parameters from the command line
 * @return The instance of the commands dispatcher
 */
CommandsDispatcher* getDispatcher(const int paramsNum, char* paramsArr[])
{
    switch(paramsNum)
	{
	  case 1:
	      printHelp(cerr, paramsArr[0], "\tMissing parameters\n");
	      break;
	  case 2:
	      saveConnectionType(BLUETOOTH);
	      return getCommandsDispatcherBT(paramsArr);
	  case 3:
	      saveConnectionType(WIFI);
	      return getCommandsDispatcherWiFi(paramsArr);
	  default:
	      printHelp(cerr, paramsArr[0], "ERROR: Invalid parameters number\n");	      
	}
    
	return NULL;    
}

/**
 * The main function of the daemon
 * @param argc There is parameter - port number
 * @param argv There is parameter - port number
 */
int main(int argc, char* argv[])
{
//	killOtherProcess();

    int exit_status = EXIT_FAILURE;
       
    spawn();
    closeStandardStreams();

    CommandsDispatcher *dispatcher = getDispatcher(argc, argv);
    if(dispatcher != NULL)
	{	    
	    exit_status = runDispatcher(dispatcher);
	    remove(FILE_NAME);
	}    
    
    exit(exit_status);
}
