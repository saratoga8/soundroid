/**
 * @file
 * The utils for work with files
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

#include "file_utils.h"

#include <dirent.h> 
#include <string.h>
#include <stdio.h>

#include <sysexits.h>
#include <errno.h>

#define POINT_SLASH "./"


/**
 * Copy a string of the give size from the file with the given path to the given buffer
 * @param filePath The path to the file
 * @param strLen The length of the string
 * @param buff The buffer for copying the string to
 * @return True or false of the operation
 */
bool cpStrFromFileToBuff(const char *filePath, size_t strLen, char *buff)
{
    bool result = false;
    
    if(buff == NULL)
	fprintf(stderr, "ERROR: The given buffer for copying string is NULL\n");
    if(filePath == NULL)
	fprintf(stderr, "ERROR: The given path for the being read file is NULL\n");
    else if(strlen(filePath) == 0)
	fprintf(stderr, "ERROR: The given path for the being read file is EMPTY\n");
    else
	{
	    FILE *file = fopen(filePath, "r");
	    if(file)
		{
		    if(fgets(buff, strLen, file) != NULL)
			result = true;
		    else
			fprintf(stderr, "ERROR: Can't copy file %s to a buffer\n", filePath);
		}
	    else
		fprintf(stderr, "ERROR: Can't open the file %s: %s\n", filePath, strerror(errno));
	    
	    if(fclose(file) != 0)
		fprintf(stderr, "ERROR: Can't close the file %s: %s\n", filePath, strerror(errno));
	}
    
    return result;
}

/**
 * Delete the file with the given path
 * @param filePath The path to the file
 */
void delFileByPath(const char *filePath)
{
    if(filePath == NULL)
	fprintf(stderr, "ERROR: The given path for the being removed file is NULL\n");
    else if(strlen(filePath) == 0)
	fprintf(stderr, "ERROR: The given path for the being removed file is EMPTY\n");
    else
	{
	    if(remove(filePath) != 0)
		fprintf(stderr, "ERROR: Can't remove the file %s: %s\n", filePath, strerror(errno));
	}
}

/**
 * Delete the characters './' from the beginning of the given string
 * @param str The string for altering
 * @return The altered string
 */
const char* delPointSlashFromStrBeg(const char *str)
{
    return strstr(str, POINT_SLASH) ? str + strlen(POINT_SLASH): str;
}

/**
 * Does the directory with the given path has the file with the given name?
 * @param dirPath The directory's path
 * @param fileName The name of the searched file
 * @return true The file is in the directory
 */
const bool doesDirHasFile(const char *dirPath, const char *fileName)
{
    if(fileName == NULL)
	{
	    fprintf(stderr, "ERROR: The given file name for searching within the current directory is NULL\n");
	    return false;
	}
    if(strlen(fileName) == 0)
	{
	    fprintf(stderr, "ERROR: The given file name for searching within the current directory is empty\n");
	    return false;
	}
    if(dirPath == NULL)
	{
	    fprintf(stderr, "ERROR: The given directory path for searching file is NULL\n");
	    return false;
	}
    if(strlen(dirPath) == 0)
	{
	    fprintf(stderr, "ERROR: The given directory path for searching file is empty\n");
	    return false;
	}

    bool result = false;
    DIR *dirStream = opendir(dirPath);
    if (dirStream)
	{
	    struct dirent *dir;
	    const char *strippedFileName = delPointSlashFromStrBeg(fileName);
	    while ((dir = readdir(dirStream)) != NULL)
		{
		    if(strcmp(dir->d_name, strippedFileName) == 0)
			{
			    result = true;
			    break;
			}
		}
	}
    else
	fprintf(stderr, "ERROR: the directory %s can't be opened: %s\n", dirPath, strerror(errno));

    if(closedir(dirStream) != 0)
	fprintf(stderr, "ERROR: can't close the directory's %s stream: %s\n", dirPath, strerror(errno));
    
    return result;
}

/**
 * Run the file with the given name and parameters in the current directory
 * The name of the file and the parameters are given in an array
 * @param argc The length of the array
 * @param argv The array
 * @return true The file has run successfully
 */
const bool runFileInCurDir(const int argc, const char *argv[argc])
{
    if(argv[0] == NULL)
	{
	    fprintf(stderr, "ERROR: The given file name for running is NULL\n");
	    return false;
	}
    if(strlen(argv[0]) == 0)
	{
	    fprintf(stderr, "ERROR: The given file name for running is empty\n");
	    return false;
	}

    size_t len = strlen(POINT_SLASH) + strlen(argv[0]) + 2;
    int i;
    for(i = 1; i < argc; ++i)
	len += strlen(argv[i]) + 1;
    
    char *command = (char*)calloc(sizeof(char), len);
    command = strcpy(command, POINT_SLASH);
    for(i = 0; i < argc; ++i)
	{
	    command = strcat(command, argv[i]);
	    command = strcat(command, " ");
	}

    const int result_code = WEXITSTATUS(system(command));
    bool result = false;
    switch(result_code) 
	{
	  case 127:
	      fprintf(stderr, "ERROR: The file '%s' hasn't found\n", command); break;	      
	  case 126:
	      fprintf(stderr, "ERROR: The file '%s' can't be executed due permisson problem or the file isn't executable\n", command); break;	      
	  case EXIT_FAILURE:
	      fprintf(stderr, "ERROR: Execution of the file '%s' has finished with failure\n", command); break;	   
	  case EXIT_SUCCESS:
	      result = true; break;	      
	  default:
	      fprintf(stderr, "ERROR: The file '%s' can't be executed: unknow error\n", command);
      }
    free(command);
    return result;
}
