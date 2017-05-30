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
#ifndef UTILS_H_
#define UTILS_H_

#include <stdbool.h>
#include <stdlib.h>

/**
 * Copy a string of the give size from the file with the given path to the given buffer
 * @param filePath The path to the file
 * @param strLen The length of the string
 * @param buff The buffer for copying the string to
 * @return True or false of the operation
 */
bool cpStrFromFileToBuff(const char *filePath, size_t strLen, char *buff);

/**
 * Delete the file with the given path
 * @param filePath The path to the file
 */
void delFileByPath(const char *filePath);

/**
 * Run the file with the given name and parameters in the current directory
 * @param argc The number of parameters
 * @param argv The array of parameters
 * @return true The file has run successfully
 */
const bool runFileInCurDir(const int argc, const char* argv[argc]);

/**
 * Does the directory with the given path has the file with the given name?
 * @param dirPath The directory's path
 * @param fileName The name of the searched file
 * @return true The file is in the directory
 */
const bool doesDirHasFile (const char *dirPath,  const char *fileName);

#endif
