#include "BlueToothLib.h"
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


void *printReceivedData()
{
    char *receivedData = NULL;
    const char* datas[] = {"hello", "false", "75", "end", "hello", "false", "50", "end"};

    printf ("Start receiving data run\n");
    int i;
    for(i = 0; i < 8; ++i)
	{
	    while(receivedData = getReceivedData())
		{
		    const size_t len = strlen(receivedData);
		    if(len != 0)
			{
			    char *str = (char*) calloc(len + 1, sizeof(char));
			    str = strcpy(str, receivedData);
			    
			    //			    if(strcmp(receivedData, "end") == 0)
			    //				{
			    //	    i = 6;
			    //				}
			    //break;
			    printf("Received: %s\n", str);
			    free(str);
			    break;
			}
		}
		
	    setSentData(datas[i]);
	    if(i == 7)
		setRunStatus(STOP);
	}

    sleep (1);

    printf("Local Name: %s\n", getLocalAddr());    
    printf("Connected Name: %s\n", getConnectedAddr());

    sleep(1);
    
    pthread_exit(NULL);

}

int main(int argc, char* argv[])
{    
//    openLogFile("log.txt");
//    printf("Opened log file\n");    

    pthread_t thread;
    int res = 0;
    if(res = pthread_create(&thread, NULL, printReceivedData, NULL))
	{
	    printf("ERROR; return code from pthread_create() is %d\n", res);
	    return -1;
	}

    const int socket = initBtConnectionBeforeListen();
    if(socket != NO_ERR)
	{
	    printf("Starting connection run\n");
	    runBtConnection(socket);
	}
    else
	printf("ERROR: Can't run connection");

    printf("Stop connection run\n");

    pthread_join(thread, NULL);
    pthread_exit(NULL);

//    closeLogFile();
//    printf("Close log file\n");
    
    printf("Exit thread\n");
    return 0;
}
