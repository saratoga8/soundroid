/**
 * Write to log file
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

#include "Log.h"

#include <errno.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include <pthread.h>

#define ERR -1                               /**< the code of an error */

FILE *logFile = NULL;                        /**< Pointer to the log file */

#define LOG_FILE_NAME_LEN 100                /**< \def The maximal length of the log file name */
char fileName[LOG_FILE_NAME_LEN] = {'\0'};   /**< The log file's name string array */

pthread_mutex_t mutex;                       /**< The mutex for concurrent writing to the same log file by different threads */

long curSize = 0;                            /**< The current size of the log */

/**
 * Change the name of the currently opened log file
 * @param name The new name
 */
void chgLogFileName(const char *name)
{
    if( (name != fileName) && (strcmp(name, fileName) != 0) )
	{
	    bzero(fileName, LOG_FILE_NAME_LEN);
	    strcpy(fileName, name);
	}
}


/**
 * Open the log file with the given name.
 * The give name will be saved in the global value fileName 
 * @param name The name of the log file
 */
void openLogFile(const char *name)
{
    if(name == NULL)
	{
	    printf("ERROR: the given file name for openning a log file is NULL\n");
	    return;
	}
    if(strlen(name) == 0)
	{
	    printf("ERROR: the given file name for openning a log file is EMPTY\n");
	    return;
	}

    chgLogFileName(name);
    
    logFile = fopen(fileName, "a+");
    if(logFile == NULL)
	{
	    printf("ERROR: can't open the log file '%s'\n", fileName);
	    perror("");
	}

    fseek(logFile, 0L, SEEK_END);
    curSize = ftell(logFile);
    
    pthread_mutex_init(&mutex, NULL);
}


/**
 * Close the log file if it has been opened before
 */
void closeLogFile()
{
    pthread_mutex_destroy(&mutex);

    if(logFile == NULL)
	{
	    printf("ERROR: can't close the log file %s, the stream pointer to one is NULL\n", fileName);
	    return;
	}
    
    if(fclose(logFile) == EOF)
	{
	    printf("ERROR: can't close the log file '%s'\n", fileName);
    	    perror("");
	}
    logFile = NULL;
}

/**
 * Get current date string pointer
 * @return The pointer to the string of current date
 */
const char* getCurDate()
{
    const time_t curTime = time(NULL);
    return ctime(&curTime);
}

/**
 * Add the current date and the given tag to the given text
 * @param txt The text
 * @param tag The tag
 * @return The new text containing the old text and the added tag and the date
 */
char* addDateToTxt(const char* txt, const char *tag)
{
    const char *date = getCurDate();
    size_t dateLen = strlen(date);

    size_t len = dateLen + strlen(":") + strlen(tag) + strlen(": ") + strlen(txt);

    char* newTxt = (char*) calloc(sizeof(char), len);

    newTxt = strncpy(newTxt, date, dateLen - 1);
    newTxt = strcat(newTxt, ":");
    newTxt = strcat(newTxt, tag);
    newTxt = strcat(newTxt, ": ");
    newTxt = strcat(newTxt, txt);

    return newTxt;
}

/**
 * Write the given text and tag to log file
 * @param txt The text for writing
 * @param tag The tag
 */
void writeToLog(const char *txt, const char *tag)
{    
    pthread_mutex_lock (&mutex);

    //    if(logFile == NULL)
    //	{
    //	    printf("ERROR: can't write to the log file %s, the stream pointer to one is NULL\n", fileName);
    //	    return;
    //	}
    openLogFile(LOG_FILE_NAME);
    
    if(curSize >= MAX_LOG_FILE_LEN)
	clearLogFile();

    if(txt == NULL)
	{
	    printf("ERROR: the given text for writing to the log file %s is NULL\n", fileName);
	    return;
	}

    if(strlen(txt) == 0)
	return;

    if(tag == NULL)
	{
	    printf("ERROR: the given tag for writing to the log file %s is NULL\n", fileName);
	    return;
	}

    if(strlen(tag) == 0)
	return;

    fflush(logFile);
    
    char* txtWithDate = addDateToTxt(txt, tag);
    size_t len = strlen(txtWithDate);
    if(fwrite(txtWithDate, sizeof(char), len, logFile) != len)
	{
	    printf("ERROR: all the given text hasn't written to the log file '%s'\n", fileName);
	    perror("");
	}
    free(txtWithDate);
    curSize += len;

    closeLogFile(LOG_FILE_NAME);
    
    pthread_mutex_unlock(&mutex);
}

/**
 * Write to log file the given two textes and tag
 * @param txt1 The first text for writing
 * @param txt2 The second text for writing
 * @param tag The tag
 */
void writeToLog2(const char *txt1, const char *txt2, const char *tag)
{
    if(txt1 == NULL)
	{
	    printf("ERROR: the given 1st part of the text for writing to the log file %s is NULL\n", fileName);
	    return;
	}
    if(txt2 == NULL)
	{
	    printf("ERROR: the given 2nd part of the text for writing to the log file %s is NULL\n", fileName);
	    return;
	}
    size_t len = strlen(txt1) + strlen(txt2) + 2;
    char *str = (char*) malloc(len);
    bzero(str, len);
    str = strcpy(str, txt1);
    str = strcat(str, txt2);
    if(txt2[strlen(txt2) - 1] != '\n')
	str[len-2] = '\n'; 
    writeToLog(str, tag);
    free(str);
}

/**
 * Clear the log file
 */
void clearLogFile()
{
    if(logFile == NULL)
	{
	    printf("ERROR: can't clear the log file %s, the stream pointer to one is NULL\n", fileName);
	    return;
	}
    closeLogFile();

    if(remove(fileName) != 0)
    	{
    	    printf("ERROR: can't remove the log file %s", fileName);
    	    perror("");
    	    return;
    	}
    
    openLogFile(fileName);
}


/**
 * Write error string to the log file
 * @param result The result index, if result is error then the given error string should be written to the log file
 * @param funcName The name of the function where the error has taken place
 * @param errorStr The error string should be written to the log file
 * @param TAG The tag string for logs
 */
void writeToLogIfError(const int result, const char *funcName, const char *errorStr, const char *TAG)
{
    if (result == ERR)
    {
	#define ERRSTR "ERROR: "
	#define STR "(): "
    	size_t len = strlen(ERRSTR) + strlen(funcName) + strlen(STR) + strlen(errorStr) + 2;
    	char *str = (char*) calloc(sizeof(char), len);
    	strcat(str, ERRSTR);
    	strcat(str, funcName);
	strcat(str, STR);
    	strcat(str, errorStr);
	strcat(str, "\n");
        writeToLog(str, TAG);
	free(str);
    }
}

/**
 * Get the size of the log file in bytes
 * @return The size of the log file in bytes
 */
long getLogSizeBytes()
{
    return curSize;
}
