/**
 * @file
 * The utils for work with a process
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

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#define PROC_PATH "/proc/"
#define FILE_NAME "/cmdline"
#define TAG "PROC_UTILS"
#define ERR -1

/**
 * Get the size of the file with the given file descriptor in bytes
 * @param fd The file's descriptor
 * @param path The path of the file
 * @return The size in bytes
 */
const int getFileSizeBytes(const int fd, const char *path)
{
    struct stat st;
    if(fstat(fd, &st) == ERR)
	{
	    writeToLog2("ERROR: Can't get the size of the file ", path, TAG);
	    writeToLog2("\tThe problem: ", strerror(errno), TAG);
	    return ERR;
	}
    return st.st_size;
}

/**
 * Get parameters string of the process with the given ID
 * @param PID The process ID
 * @return The string with the parameters or an empty string
 */
char* getProcessParams(const char* PID)
{
    const size_t len = sizeof(PROC_PATH) + sizeof(PID) + sizeof(FILE_NAME) + 1;
    char* path = (char*) calloc(len, sizeof(char));
    path = strcpy(path, PROC_PATH);
    path = strcat(path, PID);
    path = strcat(path, FILE_NAME);

    int fd = open(path, O_RDONLY);
    if(fd == ERR)
	{
	    writeToLog2("ERROR: Can't open the file ", path, TAG);
	    writeToLog2("\tThe problem: ", strerror(errno), TAG);
	    free(path);
	}

    char* str = (char*) calloc(1024, sizeof(char));
    ssize_t readBytes = read(fd, str, 1024);
    if(readBytes == ERR)
	{
	    writeToLog2("ERROR: Can't read from the file ", path, TAG);
	    writeToLog2("\tThe problem: ", strerror(errno), TAG);
	}    
    if(close(fd) == ERR)
	{
	    writeToLog2("ERROR: Can't close the file ", path, TAG);
	    writeToLog2("\tThe problem: ", strerror(errno), TAG);	    
	}
    
    int i;
    for(i = 0; i < readBytes; ++i)
	if(str[i] == '\0')
	    str[i] = ' ';

    free(path);
    return str;
}
