/**
 * Writing to log file
 * @file 
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

#ifndef __LOG_HEADER
#define __LOG_HEADER

#include <stdio.h>

/**
 * The maximal number of lines within the log file.
 * When the file has the maximal length it will be cleared
 */
#define MAX_LOG_FILE_LEN 10000

#define LOG_FILE_NAME "log.txt"                      /**< the name of the file for writing logs info */

/**
 * Open the log file with the given name
 * @param name The name of the file
 */
void openLogFile(const char *name);

/**
 * Close the log file
 */
void closeLogFile();

/**
 * Write the given text to the log file with the given tag
 * @param txt The text for adding to the log
 * @param tag The tag of the text
 */
void writeToLog(const char *txt, const char* tag);

/**
 * The given textes will be concatenated and added to the log file
 * with the given tag
 * @param txt1 The 1st text for adding to the log
 * @param txt2 The 2nd text for adding to the log
 * @param tag The tag of the text
 */
void writeToLog2(const char *txt1, const char *txt2, const char *tag);

/**
 * Clear the log file
 */
void clearLogFile();

/**
 * Write error string to the log file
 * @param result The result index, if result is error then the given error string should be written to the log file
 * @param funcName The name of the function where the error has taken place
 * @param errorStr The error string should be written to the log file
 * @param TAG The tag for logs
 */
void writeToLogIfError(const int result, const char *funcName, const char *errorStr, const char *TAG);

/**
 * Get log file's size in bytes
 */
long getLogSizeBytes();

/**
 * Add the current date and the given tag to the given text
 * @param txt The text
 * @param tag The tag for logs
 * @return The new text containing the old text and the added tag and the date
 */
char* addDateToTxt(const char* txt, const char* tag);

/**
 * Get current date string
 * @return The string of the current date
 */
const char* getCurDate();

#endif
