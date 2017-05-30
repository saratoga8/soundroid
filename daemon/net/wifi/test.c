#include "SocketsLib.h"
#include "ClosingClient.h"
#include "synchronise.h"
#include "addr.h"

#include <unistd.h>
#include <stdio.h>

#include <stdlib.h>
#include <string.h>

#include <pthread.h>

#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

#include "Log.h"

char *port = NULL;

#define TEST_PORT_STR "test-port"

void *printReceivedData()
{
    char *receivedData = NULL;
    const char* datas[] = {"test\n", "test\n", "test\n", "test\n", "test\n", "test\n", "test\n"};
    
    int i;
    for(i = 0; i < 7; ++i)
	{
	    while(true)
		{
		    receivedData = getReceivedData();
		    if(strlen(receivedData)!= 0)
			{
			    if(strcmp(receivedData, "quit") == 0)
				{
				    i = 6;
				}
			    break;
			}
		    usleep(500000);
		}
	    setSentData(datas[i]);	    
	}

    printf("Local IP: %s\n", getLocalAddr());    
    printf("Connected IP: %s\n", getConnectedAddr());

    
    setRunStatus(STOP);
    printf("Set stop\n");
    
    runClosingClient(port);
    printf("Run closing client\n");
    
    pthread_exit(NULL);

}

const int startReceiveDataThread(pthread_t *thread)
{
    int res = 0;
    if(res = pthread_create(thread, NULL, printReceivedData, NULL))
	{
	    printf("ERROR; return code from pthread_create() is %d\n", res);
	    return ERR;
	}
    printf("Run thread of receiving data\n");
    return NO_ERR;
}

int main(int argc, char* argv[])
{
    bool checkPort = false;
    if(argc == 1)
	{
	    printf("ERROR: no port number parameter!\n");
	    return 0;
	}
    else if(argc == 3)
	{
	  checkPort = true;
	    if(strstr(argv[2], TEST_PORT_STR) == NULL)
		{
		    printf("ERROR: invalid parameter '%s', should be '%s'\n", argv[2], TEST_PORT_STR);
		    return -1;
		}
	}
    
//    openLogFile("log.txt");
//    printf("Opened log file\n");    
    
    pthread_t thread;
    if(!checkPort)
	{
	    startReceiveDataThread(&thread);
	}
    
    port =  argv[1];
    if(setPort(port) == NO_ERR)
	{
	    printf("Set port: %s\n", port);
	    printf("Starting connection run\n");
	    const int socket = initConnectionBeforeListen();
	    if(socket != NO_ERR)
		runConnection(socket);
	    else
		printf("ERROR: Can't set port %s\n", port);
	}
    printf("Stop connection run\n");

    if(!checkPort)
	{
	    pthread_join(thread, NULL);
	    pthread_exit(NULL);
	}

//    closeLogFile();
//    printf("Close log file\n");
    
    printf("Exit thread\n");
    return 0;
}
